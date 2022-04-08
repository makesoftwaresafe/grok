/*
 *    Copyright (C) 2016-2022 Grok Image Compression Inc.
 *
 *    This source code is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This source code is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "grk_includes.h"

namespace grk
{
const uint8_t gain_b[4] = {0, 1, 1, 2};

ResDecompressBlocks::ResDecompressBlocks(void) : res_(0), waveletTransform_(true)
{}
void ResDecompressBlocks::clear(void){
	res_ = 0;
	blocks_.clear();
}

bool ResDecompressBlocks::empty(void) const{
	return blocks_.empty();
}

DecompressScheduler::DecompressScheduler(TileProcessor* tileProcessor,
										Tile* tile, TileCodingParams* tcp, uint8_t prec, bool doPostT1)
	: Scheduler(tile), tileProcessor_(tileProcessor), tcp_(tcp), prec_(prec), doPostT1_(doPostT1)
{
}

bool DecompressScheduler::schedule(uint16_t compno){
	auto tilec = tile_->comps + compno;

	if(!scheduleBlocks(compno))
		return false;
	auto  imageFlow = getImageComponentFlow(compno);
	if (imageFlow) {
		// composite blocks and wavelet
		imageFlow->addTo(codecFlow_);
		// generate dependency graph
		graph(compno);
	}
	uint8_t numRes = tilec->highestResolutionDecompressed + 1U;
	if(doPostT1_ && numRes > 1 && !scheduleWavelet(compno))
		return false;
	if(!run())
		return false;
	getCodecFlow().clear();

	return true;
}

bool DecompressScheduler::scheduleBlocks(uint16_t compno)
{
	blocks.clear();
	ResDecompressBlocks resBlocks;
	auto tccp = tcp_->tccps + compno;
	auto tilec = tile_->comps + compno;
	bool wholeTileDecoding = tilec->isWholeTileDecoding();
	uint8_t resno = 0;
	for(; resno <= tilec->highestResolutionDecompressed; ++resno)
	{
		auto res = tilec->tileCompResolution + resno;
		for(uint8_t bandIndex = 0; bandIndex < res->numTileBandWindows; ++bandIndex)
		{
			auto band = res->tileBand + bandIndex;
			auto paddedBandWindow =
				tilec->getBuffer()->getBandWindowPadded(resno, band->orientation);
			for(auto precinct : band->precincts)
			{
				if(!wholeTileDecoding && !paddedBandWindow->nonEmptyIntersection(precinct))
					continue;
				for(uint64_t cblkno = 0; cblkno < precinct->getNumCblks(); ++cblkno)
				{
					auto cblkBounds = precinct->getCodeBlockBounds(cblkno);
					if(wholeTileDecoding || paddedBandWindow->nonEmptyIntersection(&cblkBounds))
					{
						auto cblk = precinct->getDecompressedBlockPtr(cblkno);
						auto block = new DecompressBlockExec();
						block->x = cblk->x0;
						block->y = cblk->y0;
						block->tilec = tilec;
						block->bandIndex = bandIndex;
						block->bandNumbps = band->numbps;
						block->bandOrientation = band->orientation;
						block->cblk = cblk;
						block->cblk_sty = tccp->cblk_sty;
						block->qmfbid = tccp->qmfbid;
						block->resno = resno;
						block->roishift = tccp->roishift;
						block->stepsize = band->stepsize;
						block->k_msbs = (uint8_t)(band->numbps - cblk->numbps);
						block->R_b = prec_ + gain_b[band->orientation];
						resBlocks.blocks_.push_back(block);
					}
				}
			}
		}
		// combine first two resolutions together into single resBlock
		if(!resBlocks.blocks_.empty() && resno > 0)
		{
			resBlocks.res_ = resno-1;
			blocks.push_back(resBlocks);
			resBlocks.clear();
		}
	}
	// handle case where only one resolution is decompressed
	// (in this case, there will be no wavelet transform)
	if(!resBlocks.empty())
	{
		assert(tilec->highestResolutionDecompressed == 0);
		resBlocks.res_ = 0;
		resBlocks.waveletTransform_ = false;
		blocks.push_back(resBlocks);
		resBlocks.clear();
	}
	if(!blocks.size())
		return true;

	uint8_t numResolutions = (tile_->comps + compno)->highestResolutionDecompressed + 1;
	imageComponentFlows_[compno] = new ImageComponentFlow(numResolutions);
	if (!tile_->comps->isWholeTileDecoding())
		imageComponentFlows_[compno]->setRegionDecompression();

	// nominal code block dimensions
	uint16_t codeblock_width = (uint16_t)(tccp->cblkw ? (uint32_t)1 << tccp->cblkw : 0);
	uint16_t codeblock_height = (uint16_t)(tccp->cblkh ? (uint32_t)1 << tccp->cblkh : 0);
	for(auto i = 0U; i < ExecSingleton::get()->num_workers(); ++i)
		t1Implementations.push_back(
			T1Factory::makeT1(false, tcp_, codeblock_width, codeblock_height));

	size_t num_threads = ExecSingleton::get()->num_workers();
	success = true;
	if(num_threads == 1)
	{
		for(auto& resBlocks : blocks)
		{
			for(auto& block : resBlocks.blocks_)
			{
				if(!success)
				{
					delete block;
				}
				else
				{
					auto impl = t1Implementations[(size_t)0];
					if(!decompressBlock(impl, block))
						success = false;
				}
			}
		}

		return success;
	}
	uint8_t resFlowNum = 0;
	for(auto& resBlocks : blocks)
	{
		auto resFlow = imageComponentFlows_[compno]->resFlows_ + resFlowNum;
		for(auto& block : resBlocks.blocks_)
		{
			resFlow->blocks_->nextTask()->work([this, block] {
				if(!success)
				{
					delete block;
				}
				else
				{
					auto threadnum = ExecSingleton::get()->this_worker_id();
					auto impl = t1Implementations[(size_t)threadnum];
					if(!decompressBlock(impl, block))
						success = false;
				}
			});
		}
		resFlowNum++;
	}

	return true;
}
bool DecompressScheduler::decompressBlock(T1Interface* impl, DecompressBlockExec* block)
{
	try
	{
		bool rc = block->open(impl);
		delete block;
		return rc;
	}
	catch(std::runtime_error& rerr)
	{
		delete block;
		GRK_ERROR(rerr.what());
		return false;
	}

	return true;
}

bool DecompressScheduler::scheduleWavelet(uint16_t compno) {
	auto tilec = tile_->comps + compno;
	uint8_t numRes = tilec->highestResolutionDecompressed + 1U;
	WaveletReverse w(tileProcessor_, tilec,
					compno, tilec->getBuffer()->unreducedBounds(), numRes,
					 (tcp_->tccps + compno)->qmfbid);

	return w.decompress();

	return false;
}


} // namespace grk
