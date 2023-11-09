#pragma once

class Clock
{
public:
	static inline float s_globalClock{ 0.0f };

	Clock() = default;
	Clock(const float initialPhase) : m_triggerClock{ initialPhase }, m_executionClock{ initialPhase } {}
#if defined (CLOCK)
	inline void tickGlobalClock() { s_globalClock++; }

	inline void tickTriggerClock() { m_triggerClock++; }
	inline void tickTriggerClock(const float cycles) { m_triggerClock += cycles; }
	inline void tickTriggerClock(const float cycles) const { m_triggerClock += cycles; }
	inline bool triggerLocalEvent() { return ((s_globalClock >= m_triggerClock) ? true : false); }
	inline bool triggerLocalEvent() const { return ((s_globalClock >= m_triggerClock) ? true : false); }
	inline void synchronizeTriggerClock() { if (s_globalClock == m_triggerClock) m_triggerClock++; }
	inline void synchronizeTriggerClock() const { if (s_globalClock == m_triggerClock) m_triggerClock++; }

	inline void tickExecutionClock() { m_executionClock++; }
	inline void tickExecutionClock(const float cycles) { m_executionClock += cycles; }
	inline void tickExecutionClock(const float cycles) const { m_executionClock += cycles; }
	inline bool executeLocalEvent() { return ((s_globalClock >= m_executionClock) ? true : false); }
	inline bool executeLocalEvent() const { return ((s_globalClock >= m_executionClock) ? true : false); }
	inline void synchronizeExecutionClock() { if (s_globalClock == m_executionClock) m_executionClock++; }
	inline void synchronizeExecutionClock() const { if (s_globalClock == m_executionClock) m_executionClock++; }

	inline void toggleWaitingForExecution() { m_waitingForExecution = !m_waitingForExecution; }
	inline void toggleWaitingForExecution() const { m_waitingForExecution = !m_waitingForExecution; }
	inline bool isWaitingForExecution() { return m_waitingForExecution; }
	inline bool isWaitingForExecution() const { return m_waitingForExecution; }
#else 
	inline void tickGlobalClock() { }

	inline void tickTriggerClock() { }
	inline void tickTriggerClock(const float cycles) { }
	inline void tickTriggerClock(const float cycles) const { }
	inline bool triggerLocalEvent() { return true; }
	inline bool triggerLocalEvent() const { return true; }
	inline void synchronizeTriggerClock() { }
	inline void synchronizeTriggerClock() const { }

	inline void tickExecutionClock() { }
	inline void tickExecutionClock(const float cycles) { }
	inline void tickExecutionClock(const float cycles) const { }
	inline bool executeLocalEvent() { return true; }
	inline bool executeLocalEvent() const { return true; }
	inline void synchronizeExecutionClock() { }
	inline void synchronizeExecutionClock() const { }

	inline void toggleWaitingForExecution() { }
	inline void toggleWaitingForExecution() const { }
	inline bool isWaitingForExecution() { return false; }
	inline bool isWaitingForExecution() const { return false; }
#endif

private:
	mutable bool m_waitingForExecution{ false };
	mutable float m_triggerClock{ 0.0f };
	mutable float m_executionClock{ 0.0f };
};