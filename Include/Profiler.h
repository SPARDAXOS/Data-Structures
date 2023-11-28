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

class ProfilingUnit final {
public:
	explicit ProfilingUnit() noexcept = default;
	ProfilingUnit(std::string_view id, Timepoint start)
		: m_ID(id), m_StartingTimepoint(start)
	{
	}
	ProfilingUnit(std::string_view id, Timepoint start, Timepoint end, Duration duration)
		: m_ID(id), m_StartingTimepoint(start), m_EndTimepoint(end), m_Duration(duration)
	{
	}

public:
	inline Microseconds AsMicroseconds() const noexcept { return std::chrono::duration_cast<Microseconds>(m_Duration); }
	inline Milliseconds AsMilliseconds() const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Duration); }
	inline Seconds AsSeconds() const noexcept { return std::chrono::duration_cast<Seconds>(m_Duration); }

public:
	std::string_view m_ID;
	Timepoint m_StartingTimepoint;
	Timepoint m_EndTimepoint;
	Duration m_Duration;
};


class Profiler final {
public:
	explicit Profiler() noexcept = default;
	~Profiler() noexcept = default;

	Profiler(const Profiler& other) = default;
	Profiler& operator=(const Profiler& other) = default;

	Profiler(Profiler&& other) noexcept = default;
	Profiler& operator=(Profiler&& other) noexcept = default;

public:
	inline bool StartProfile(std::string_view id) {
		if (FindUnit(id).has_value()) {
			std::cout << "Profile unit with id " << id << " already exists!" << std::endl;
			return false;
		}

		m_ProfillingUnits.emplace_back(id, m_Clock.now());
		m_RunningProfiles++;
		return true;
	}
	[[nodiscard]] inline std::optional<ProfilingUnit> EndProfile(std::string_view id) {
		auto TargetUnit = FindUnit(id);
		if (!TargetUnit.has_value()) {
			std::cout << "Failed to find and end profiling unit designated " << id << std::endl;
			return std::optional<ProfilingUnit>(std::nullopt);
		}

		TargetUnit->m_EndTimepoint = m_Clock.now();
		TargetUnit->m_Duration = TargetUnit->m_EndTimepoint - TargetUnit->m_StartingTimepoint;

		//m_ProfillingUnits.Erase(std::addressof(TargetUnit.value())); //UB    !!!!
		m_RunningProfiles--;
		return TargetUnit;
	}


private:
	inline std::optional<ProfilingUnit> FindUnit(std::string_view id) const noexcept {
		for (auto& unit : m_ProfillingUnits) {
			if (unit.m_ID == id)
				return std::optional<ProfilingUnit>(unit);
		}

		return std::optional<ProfilingUnit>(std::nullopt);
	}
	

private:
	std::chrono::high_resolution_clock m_Clock;
	Container<ProfilingUnit> m_ProfillingUnits;
	uint32 m_RunningProfiles = 0;
};
#endif // !PROFILER
