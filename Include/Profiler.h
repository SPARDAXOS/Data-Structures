#include "Container.h"
#include "Utility.h"
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
class DeepProfile final {
public:
	explicit DeepProfile() noexcept = default;
	DeepProfile(Timepoint start)
		: m_StartingTimepoint(start)
	{
	}

public: //Helpers


public:
	Timepoint m_StartingTimepoint;
	Timepoint m_EndTimepoint;
	Duration m_Duration{ 0 };
	Duration m_Average{ 0 };
	Duration m_Median{ 0 };
	Duration m_Max{ 0 };
	Duration m_Min{ 0 };
	Container<Profile> m_Profiles;
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
	[[nodiscard]] inline Profile QuickProfile(Predicate block, uint32 iterations = 1) {
		if (!StartProfile("QuickProfile"))
			return Profile();

		//Calculate time accumulated from updating profile data such as profile insertions
		Duration InsertionsTime;
		for (uint32 i = 0; i < iterations; i++) {
			block();

		}

		auto Results = EndProfile("QuickProfile");
		if (!Results.has_value())
			return Profile();

		return *Results;
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
