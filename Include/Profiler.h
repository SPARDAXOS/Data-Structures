#include "Container.h"
#include "Utility.h"
#include "Sorting.h"
#include <chrono>
#include <iostream>

#ifndef PROFILER
#define PROFILER

namespace {

	using Timepoint = std::chrono::high_resolution_clock::time_point;
	using Duration = std::chrono::high_resolution_clock::duration;
	using Microseconds = std::chrono::microseconds;
	using Milliseconds = std::chrono::milliseconds;
	using Seconds = std::chrono::seconds;
	using Minutes = std::chrono::minutes;
	using Hours = std::chrono::hours;

}

class Profile final {
public:
	explicit Profile() noexcept = default;
	Profile(std::string_view id, Timepoint start)
		: m_ID(id), m_StartingTimepoint(start)
	{
	}
	Profile(std::string_view id, Timepoint start, Timepoint end, Duration duration)
		: m_ID(id), m_StartingTimepoint(start), m_EndTimepoint(end), m_Duration(duration)
	{
	}

	bool operator<(const Profile& other) const noexcept {
		return this->m_Duration < other.m_Duration;
	}
	bool operator>(const Profile& other) const noexcept {
		return this->m_Duration > other.m_Duration;
	}

public:
	inline Microseconds DurationAsMicroseconds() const noexcept { return std::chrono::duration_cast<Microseconds>(m_Duration); }
	inline Milliseconds DurationAsMilliseconds() const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Duration); }
	inline Seconds DurationAsSeconds() const noexcept { return std::chrono::duration_cast<Seconds>(m_Duration); }

public:
	std::string_view m_ID;
	Timepoint m_StartingTimepoint;
	Timepoint m_EndTimepoint;
	Duration m_Duration{ 0 };
};
class IterativeProfile final {
public:
	IterativeProfile() noexcept = default;
	IterativeProfile(Timepoint start)
		: m_StartingTimepoint(start)
	{
	}

public:
	inline Microseconds DurationAsMicroseconds() const noexcept { return std::chrono::duration_cast<Microseconds>(m_Duration); }
	inline Milliseconds DurationAsMilliseconds() const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Duration); }
	inline Seconds DurationAsSeconds() const noexcept { return std::chrono::duration_cast<Seconds>(m_Duration); }

public:
	Timepoint m_StartingTimepoint;
	Timepoint m_EndTimepoint;
	Duration m_Duration{ 0 };
	Duration m_Average{ 0 };
	Duration m_Median{ 0 };
	Duration m_Max{ 0 };
	Duration m_Min{ 0 };
	uint32 m_Count{ 0 };
	std::vector<Profile> m_Profiles;
};


class Profiler final {
public:
	using Predicate = std::function<void(void)>;

public:
	explicit Profiler() noexcept = default;
	~Profiler() noexcept = default;

	Profiler(const Profiler& other) = default;
	Profiler& operator=(const Profiler& other) = default;

	Profiler(Profiler&& other) noexcept = default;
	Profiler& operator=(Profiler&& other) noexcept = default;

public:
	inline bool StartProfile(std::string_view id) {
		if (FindUnit(std::string(id)).has_value()) {
			std::cout << "Profile unit with id " << id << " already exists!" << std::endl;
			return false;
		}

		m_ProfillingUnits.emplace_back(id, m_Clock.now());
		m_RunningProfiles++;
		return true;
	}
	[[nodiscard]] inline std::optional<Profile> EndProfile(std::string_view id) noexcept {
		auto StoppingTime = m_Clock.now();
		auto TargetUnit = FindUnit(std::string(id));
		if (!TargetUnit.has_value()) {
			std::cout << "Failed to find and end profiling unit designated " << id << std::endl;
			return std::optional<Profile>(std::nullopt);
		}

		TargetUnit->m_EndTimepoint = StoppingTime;
		TargetUnit->m_Duration = TargetUnit->m_EndTimepoint - TargetUnit->m_StartingTimepoint;

		//m_ProfillingUnits.Erase(std::addressof(TargetUnit.value())); //UB    !!!!
		m_RunningProfiles--;
		return TargetUnit;
	}
	[[nodiscard]] inline IterativeProfile RunIterativeProfile(Predicate block, uint32 iterations = 1) {
		if (iterations == 0)
			return {};

		IterativeProfile ResultsProfile;
		ResultsProfile.m_Count = iterations;
		ResultsProfile.m_Profiles.reserve(iterations);
		ResultsProfile.m_StartingTimepoint = m_Clock.now();
		
		for (uint32 i = 0; i < iterations; i++) {
			Profile NewProfile;
			NewProfile.m_StartingTimepoint = m_Clock.now();
			block();
			NewProfile.m_EndTimepoint = m_Clock.now();
			NewProfile.m_Duration = NewProfile.m_EndTimepoint - NewProfile.m_StartingTimepoint;
			ResultsProfile.m_Duration += NewProfile.m_Duration;
			ResultsProfile.m_Profiles.emplace_back(NewProfile);

			if (i == 0) {
				ResultsProfile.m_Max = NewProfile.m_Duration;
				ResultsProfile.m_Min = NewProfile.m_Duration;
				continue;
			}

			if (ResultsProfile.m_Min > NewProfile.m_Duration)
				ResultsProfile.m_Min = NewProfile.m_Duration;
			if (ResultsProfile.m_Max < NewProfile.m_Duration)
				ResultsProfile.m_Max = NewProfile.m_Duration;
		}

		ResultsProfile.m_EndTimepoint = m_Clock.now();
		ResultsProfile.m_Average = ResultsProfile.m_Duration / ResultsProfile.m_Profiles.size();

		//TEST MERGE SORT !!!!!B IT DIDNT WORK HERE I THINK? Quick works. Expand to actually check the right number!!!
		Sorting::MergeSort(ResultsProfile.m_Profiles.begin()._Ptr, ResultsProfile.m_Profiles.end()._Ptr);
		Sorting::QuickSort(ResultsProfile.m_Profiles.begin()._Ptr, ResultsProfile.m_Profiles.end()._Ptr);
		if (ResultsProfile.m_Profiles.size() % 2 == 1) {
			ResultsProfile.m_Median = ResultsProfile.m_Profiles[(ResultsProfile.m_Profiles.size() + 1) / 2].m_Duration; //IF THEY ARE SORTED!
		}
		else if (ResultsProfile.m_Profiles.size() % 2 == 0) {

		}

		//Calc median

		return ResultsProfile;
	}

private:
	inline std::optional<Profile> FindUnit(std::string id) const noexcept {
		for (auto& unit : m_ProfillingUnits) {
			if (unit.m_ID == id)
				return std::optional<Profile>(unit);
		}

		return std::optional<Profile>(std::nullopt);
	}
	
private:
	std::chrono::high_resolution_clock m_Clock;
	Container<Profile> m_ProfillingUnits;
	uint32 m_RunningProfiles = 0;
};
#endif // !PROFILER
