#pragma once

#include <xul/hash/sha1.hpp>
#include <xul/data/big_number.hpp>
#include <xul/hash/sha1.hpp>
#include <stdint.h>
#include <assert.h>


namespace xul
{
	typedef xul::big_number<20> sha1_hash;

	class hasher
	{
	public:
		static sha1_hash hash(const char* data, int len)
		{
			hasher h(data, len);
			return h.final();
		}
		static sha1_hash hash(std::string const& data)
		{
			return hash(data.data(), data.size());
		}
		static sha1_hash hash(const unsigned char* data, int len)
		{
			return hash(reinterpret_cast<const char*>(data), len);
		}

		hasher() { SHA1_Init(&m_context); }
		hasher(const char* data, int len)
		{
			SHA1_Init(&m_context);
			assert(data != 0);
			assert(len > 0);
			SHA1_Update(&m_context, reinterpret_cast<unsigned char const*>(data), len);
		}
		void update(std::string const& data) { update(&data[0], data.size()); }
		void update(const char* data, int len)
		{
			assert(data != 0);
			assert(len > 0);
			SHA1_Update(&m_context, reinterpret_cast<unsigned char const*>(data), len);
		}

		sha1_hash final()
		{
			sha1_hash digest;
			SHA1_Final(digest.begin(), &m_context);
			return digest;
		}

		void reset() { SHA1_Init(&m_context); }
	private:
		SHA_CTX m_context;
	};
}
