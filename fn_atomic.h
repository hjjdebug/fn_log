#pragma once
#include <atomic>
namespace FNLog
{
	template<class Mutex>
		class AutoGuard
		{
			public:
				using mutex_type = Mutex;
				inline explicit AutoGuard(Mutex& mtx, bool noop = false) : mutex_(mtx), noop_(noop) 
			{
				if (!noop_)
				{
					mutex_.lock();
				}
			}
				inline ~AutoGuard() noexcept
				{ 
					if (!noop_)
					{
						mutex_.unlock();
					}
				}
				AutoGuard(const AutoGuard&) = delete;
				AutoGuard& operator=(const AutoGuard&) = delete;
			private:
				Mutex& mutex_;
				bool noop_;
		};
	template<class V>
		inline V FN_MIN(V x, V y) 
		{
			return x < y ? x : y;
		}
	template<class V>
		inline V FN_MAX(V x, V y)
		{
			return x < y ? y : x;
		}
#ifdef FN_LOG_USING_ATOM_CFG
#define AtomicLoadC(m, eid) m.config_fields_[eid].load(std::memory_order_relaxed)
#else //使用的是下面, 获取到一个整形值
#define AtomicLoadC(m, eid) m.config_fields_[eid]
#endif // FN_LOG_USING_ATOM_CFG
	template <class M>
		inline long long AtomicLoadL(M& m, unsigned eid)
		{
			return m.log_fields_[eid].load(std::memory_order_relaxed);
		}
	template <class M>
		inline void AtomicAddL(M& m, unsigned eid)
		{
			m.log_fields_[eid].fetch_add(1, std::memory_order_relaxed);
		}
	template <class M>
		inline void AtomicAddLV(M& m, unsigned eid, long long v)
		{
			m.log_fields_[eid].fetch_add(v, std::memory_order_relaxed);
		}
	template <class M>
		inline void AtomicStoreL(M& m, unsigned eid, long long v)
		{
			m.log_fields_[eid].store(v, std::memory_order_relaxed);
		}
}
