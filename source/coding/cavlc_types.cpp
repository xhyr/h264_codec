#include "cavlc_types.h"

#include <numeric>

__codec_begin

void LevelAndRuns::Clear()
{
	levels.clear();
	runs.clear();
}

int LevelAndRuns::GetNonZeroNum() const
{
    return levels.size();
}

int LevelAndRuns::GetTrailingOnes() const
{
	int cnt = 0;
	for (auto iter = levels.rbegin(); iter != levels.rend() && cnt < 3; ++iter)
	{
		if (*iter == 1 || *iter == -1)
			++cnt;
		else break;
	}
	return cnt;
}

int LevelAndRuns::GetTotalZeros() const
{
	return std::accumulate(runs.begin(), runs.end(), 0);
}

__codec_end
