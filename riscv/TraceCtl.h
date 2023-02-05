#pragma once

class TraceCtl {
public:
	TraceCtl() = default;

	bool idebug() const { return m_debug_flags & (1<<1); }

	bool mtrace() const { return m_trace_flags & (1<<1); }
	bool ptrace() const { return m_trace_flags & (1<<2); }

	void set_trace_flag(int w) { m_trace_flags |= 1 << w; }
	void set_trace_flags(int f) { m_trace_flags = f; }

	void set_debug_flag(int w) { m_debug_flags |= 1 << w; }
	void set_debug_flags(int f) { m_debug_flags = f; }

private:
	static int m_trace_flags;
	static int m_debug_flags;
};
