#include "global.h"
#include "NotesWriterPadmiss.h"
#include "TimingData.h"
#include "json/value.h"
#include "JsonUtil.h"
#include "Song.h"
#include "BackgroundUtil.h"
#include "Steps.h"
#include "NoteData.h"
#include "GameManager.h"

static void Serialize(const TimingSegment &seg, Json::Value &root)
{
	root["beat"] = seg.GetBeat();

	if (seg.GetType() == SEGMENT_BPM)
	{
		root["bpm"] = static_cast<BPMSegment &>(const_cast<TimingSegment &>(seg)).GetBPM();
	}
	else if(seg.GetType() == SEGMENT_STOP)
	{
		root["duration"] = static_cast<StopSegment &>(const_cast<TimingSegment &>(seg)).GetPause();
	}
	else if(seg.GetType() == SEGMENT_DELAY)
	{
		root["duration"] = static_cast<DelaySegment &>(const_cast<TimingSegment &>(seg)).GetPause();
	}
	else if (seg.GetType() == SEGMENT_WARP)
	{
		root["length"] = static_cast<WarpSegment&>(const_cast<TimingSegment&>(seg)).GetLength();
	}
	else if (seg.GetType() == SEGMENT_TIME_SIG)
	{
		auto segment = static_cast<TimeSignatureSegment&>(const_cast<TimingSegment&>(seg));
		root["notesPerMeasure"] = segment.GetNoteRowsPerMeasure();
		//todo not sure
		root["noteValue"] = segment.GetDen();
	}
	else if (seg.GetType() == SEGMENT_TICKCOUNT)
	{
		root["ticksPerBeat"] = static_cast<TickcountSegment&>(const_cast<TimingSegment&>(seg)).GetTicks();
	}
	else if (seg.GetType() == SEGMENT_COMBO)
	{
		auto segment = static_cast<ComboSegment&>(const_cast<TimingSegment&>(seg));
		root["comboAmount"] = segment.GetCombo();
		root["missComboAmount"] = segment.GetMissCombo();
	}
	else if (seg.GetType() == SEGMENT_SCROLL)
	{
		root["ratio"] = static_cast<ScrollSegment&>(const_cast<TimingSegment&>(seg)).GetRatio();
	}
	else if (seg.GetType() == SEGMENT_FAKE)
	{
		root["length"] = static_cast<FakeSegment&>(const_cast<TimingSegment&>(seg)).GetLength();
	}
	else if (seg.GetType() == SEGMENT_LABEL)
	{
		root["label"] = static_cast<LabelSegment&>(const_cast<TimingSegment&>(seg)).GetLabel();
	}
}

static void Serialize( const TapNote &o, Json::Value &root )
{
	root = Json::Value(Json::objectValue);

	if( o.type != TapNoteType_Tap )
		root["Type"] = (int)o.type;
	if( o.type == TapNoteType_HoldHead )
		root["SubType"] = (int)o.subType;
	//root["Source"] = (int)source;
	if( !o.sAttackModifiers.empty() )
		root["AttackModifiers"] = o.sAttackModifiers;
	if( o.fAttackDurationSeconds > 0 )
		root["AttackDurationSeconds"] = o.fAttackDurationSeconds;
	if( o.iKeysoundIndex != -1 )
		root["KeysoundIndex"] = o.iKeysoundIndex;
	if( o.iDuration > 0 )
		root["Duration"] = o.iDuration;
	if( o.pn != PLAYER_INVALID )
		root["PlayerNumber"] = (int)o.pn;
}

static void Serialize( const Steps& chart, Json::Value &measureArray )
{
	RString chartString;
	chart.GetSMNoteData(chartString);

	vector<RString> measures;
	split(chartString, ",", measures, true );

	measureArray = Json::Value(Json::arrayValue);

	for( auto measure : measures )
	{
		vector<RString> noteRows;
		split(measure, "\n", noteRows, true);

		measureArray.resize(measureArray.size() + 1);
		Json::Value& measureItem = measureArray[measureArray.size() - 1];
		measureItem["notes"] = Json::Value(Json::arrayValue);

		for (auto noteRow : noteRows)
		{
			Trim(noteRow);
			if (noteRow.length() == 0) {
				continue;
			}

			measureItem["notes"].resize(measureItem["notes"].size() + 1);
			Json::Value& rowItem = measureItem["notes"][measureItem["notes"].size() - 1];
			rowItem = noteRow;
		}
	}
}

bool NotesWriterPadmiss::WriteSteps( const RString &sFile, const Steps &chart )
{
	Song &song = *chart.m_pSong;

	Json::Value root;

	root["version"] = "1.0";
	root["playMode"] = "unknown";

	root["chartName"] = chart.GetChartName();
	root["chartDescription"] = chart.GetDescription();
	root["chartStyle"] = chart.GetChartStyle();
	root["chartDifficulty"] = chart.GetDifficulty();
	root["chartDifficultyLevel"] = chart.GetMeter();
	root["stepArtistName"] = chart.GetCredit();

	Serialize(chart, root["measures"]);

	root["songPack"] = song.m_sGroupName;
	root["songTitle"] = song.m_sMainTitle;
	root["songSubTitle"] = song.m_sSubTitle;
	root["songArtist"] = song.m_sArtist;
	root["songTitleTranslation"] = song.m_sMainTitleTranslit;
	root["songSubTitleTranslation"] = song.m_sSubTitleTranslit;
	root["songArtistTranslation"] = song.m_sArtistTranslit;
	root["songGenre"] = song.m_sGenre;

	auto timingData = chart.GetTimingData();
	root["musicOffset"] = timingData->m_fBeat0OffsetInSeconds;

	if (song.m_fSpecifiedBPMMin == song.m_fSpecifiedBPMMax) {
		root["displayBpm"] = std::to_string(song.m_fSpecifiedBPMMin);
	}
	else {
		root["displayBpm"] = std::to_string(song.m_fSpecifiedBPMMin) + " - " + std::to_string(song.m_fSpecifiedBPMMax);
	}

	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_BPM), Serialize, root["bpms"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_STOP), Serialize, root["stops"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_DELAY), Serialize, root["delays"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_WARP), Serialize, root["warps"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_TIME_SIG), Serialize, root["timeSignatures"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_TICKCOUNT), Serialize, root["ticks"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_COMBO), Serialize, root["combos"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_COMBO), Serialize, root["scrolls"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_COMBO), Serialize, root["fakes"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_COMBO), Serialize, root["labels"]);
	
	return JsonUtil::WriteFile(root, sFile, false);
}

/*
 * (c) 2001-2004 Chris Danford, Glenn Maynard
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
