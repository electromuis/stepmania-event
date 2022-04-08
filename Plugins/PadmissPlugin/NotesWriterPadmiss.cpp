#include "global.h"
#include "NotesWriterPadmiss.h"
#include "TimingData.h"

#include "JsonUtil.h"
#include "Song.h"
#include "BackgroundUtil.h"
#include "Steps.h"
#include "NoteData.h"
#include "GameManager.h"
#include "PluginManager.h"

static void Serialize(const TimingSegment& seg, Json::Value& root)
{
	root["beat"] = seg.GetBeat();

	if (seg.GetType() == SEGMENT_BPM)
	{
		root["bpm"] = static_cast<BPMSegment&>(const_cast<TimingSegment&>(seg)).GetBPM();
	}
	else if (seg.GetType() == SEGMENT_STOP)
	{
		root["duration"] = static_cast<StopSegment&>(const_cast<TimingSegment&>(seg)).GetPause();
	}
	else if (seg.GetType() == SEGMENT_DELAY)
	{
		root["duration"] = static_cast<DelaySegment&>(const_cast<TimingSegment&>(seg)).GetPause();
	}
	else if (seg.GetType() == SEGMENT_WARP)
	{
		root["length"] = static_cast<WarpSegment&>(const_cast<TimingSegment&>(seg)).GetLength();
	}
	else if (seg.GetType() == SEGMENT_TIME_SIG)
	{
		auto segment = static_cast<TimeSignatureSegment&>(const_cast<TimingSegment&>(seg));
		root["notesPerMeasure"] = segment.GetNum();
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

static void Serialize(const TapNote& o, Json::Value& root)
{
	root = Json::Value(Json::objectValue);

	if (o.type != TapNoteType_Tap)
		root["Type"] = (int)o.type;
	if (o.type == TapNoteType_HoldHead)
		root["SubType"] = (int)o.subType;
	//root["Source"] = (int)source;
	if (!o.sAttackModifiers.empty())
		root["AttackModifiers"] = o.sAttackModifiers;
	if (o.fAttackDurationSeconds > 0)
		root["AttackDurationSeconds"] = o.fAttackDurationSeconds;
	if (o.iKeysoundIndex != -1)
		root["KeysoundIndex"] = o.iKeysoundIndex;
	if (o.iDuration > 0)
		root["Duration"] = o.iDuration;
	if (o.pn != PLAYER_INVALID)
		root["PlayerNumber"] = (int)o.pn;
}

static void Serialize(const Steps& chart, Json::Value& measureArray)
{
	chart.Decompress();
	measureArray = Json::Value(Json::arrayValue);

	

	void* stringSpace = PLUGINMAN->AppAllocate(sizeof(RString));
	RString* chartString = new (stringSpace)RString();
	chart.GetSMNoteData(*chartString);

	void* vectorSpace = PLUGINMAN->AppAllocate(sizeof(vector<RString>));
	vector<RString>* measures = new (vectorSpace)vector<RString>();

	split(*chartString, ",", *measures, true);

	for (auto measure : *measures)
	{
		void* noteSpace = PLUGINMAN->AppAllocate(sizeof(vector<RString>));
		vector<RString>* noteRows = new (noteSpace)vector<RString>();
		split(measure, "\n", *noteRows, true);

		measureArray.resize(measureArray.size() + 1);
		Json::Value& measureItem = measureArray[measureArray.size() - 1];
		measureItem["notes"] = Json::Value(Json::arrayValue);

		for (auto noteRow : *noteRows)
		{
			Trim(noteRow);
			if (noteRow.length() == 0) {
				continue;
			}

			measureItem["notes"].resize(measureItem["notes"].size() + 1);
			Json::Value& rowItem = measureItem["notes"][measureItem["notes"].size() - 1];
			rowItem = noteRow;
		}

		PLUGINMAN->AppFree(noteSpace);
	}

	PLUGINMAN->AppFree(stringSpace);
	PLUGINMAN->AppFree(vectorSpace);
}

bool NotesWriterPadmiss::WriteSong(Json::Value& root, const Song& song)
{
	root["songPack"] = song.m_sGroupName;
	root["songTitle"] = song.m_sMainTitle;
	root["songSubTitle"] = song.m_sSubTitle;
	root["songArtist"] = song.m_sArtist;
	root["songTitleTranslation"] = song.m_sMainTitleTranslit;
	root["songSubTitleTranslation"] = song.m_sSubTitleTranslit;
	root["songArtistTranslation"] = song.m_sArtistTranslit;
	root["songGenre"] = song.m_sGenre;

	if (song.m_fSpecifiedBPMMin == song.m_fSpecifiedBPMMax) {
		root["displayBpm"] = std::to_string(song.m_fSpecifiedBPMMin);
	}
	else {
		root["displayBpm"] = std::to_string(song.m_fSpecifiedBPMMin) + " - " + std::to_string(song.m_fSpecifiedBPMMax);
	}

	return true;
}

bool NotesWriterPadmiss::WriteSteps(Json::Value& root, const Steps& chart)
{
	root["version"] = "1.0";
	root["playMode"] = "Single";

	root["chartName"] = chart.GetChartName();
	root["chartDescription"] = chart.GetDescription();
	root["chartStyle"] = chart.GetChartStyle();
	root["chartDifficulty"] = DifficultyToString(chart.GetDifficulty());
	root["chartDifficultyLevel"] = chart.GetMeter();
	root["stepArtistName"] = chart.GetCredit();

	Serialize(chart, root["measures"]);

	auto timingData = chart.GetTimingData();
	root["musicOffset"] = timingData->m_fBeat0OffsetInSeconds;
	
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_BPM), Serialize, root["bpms"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_STOP), Serialize, root["stops"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_DELAY), Serialize, root["delays"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_WARP), Serialize, root["warps"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_TIME_SIG), Serialize, root["timeSignatures"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_TICKCOUNT), Serialize, root["ticks"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_COMBO), Serialize, root["combos"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_SCROLL), Serialize, root["scrolls"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_FAKE), Serialize, root["fakes"]);
	JsonUtil::SerializeVectorPointers(timingData->GetTimingSegments(SEGMENT_LABEL), Serialize, root["labels"]);

	return true;
}
