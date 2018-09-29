#include <iterator>

namespace sg14
{
	template<typename OutIt, typename Allocator = std::allocator<char>>
	class lzw_compressor : public OutIt
	{
	public:
		lzw_compressor(OutIt&& dest_, int bits_);
		lzw_compressor& operator*() { return *this; }
		lzw_compressor& operator=(const char&);

	private:
		Allocator allocator;
		int bits;
	};

	template<typename OutIt, typename Allocator = std::allocator<char>>
	class lzw_decompressor : public OutIt
	{
	public:
		lzw_decompressor(OutIt&& dest_);
		lzw_decompressor& operator*() { return *this; }
		lzw_decompressor& operator=(const char&);

	private:
		Allocator allocator;
	};

	template<typename OutIt>
	class rsa_encryptor : public OutIt
	{
	public:
		rsa_encryptor(OutIt&& dest_, int m_, int n_);
		rsa_encryptor& operator*() { return *this; }
		rsa_encryptor& operator=(const char&);

	private:
		int m;
		int n;
	};

	template<typename OutIt>
	class rsa_decryptor : public OutIt
	{
	public:
		rsa_decryptor(OutIt&& dest_, int m_, int n_);
		rsa_decryptor& operator*() { return *this; }
		rsa_decryptor& operator=(const char&);

	private:
		int m;
		int n;
	};
}

template<typename OutIt, typename Allocator>
sg14::lzw_compressor<OutIt, Allocator>::lzw_compressor(OutIt&& dest_, int bits_)
	: OutIt(std::move(dest_))
	, bits(bits_)
{}

template<typename OutIt, typename Allocator>
sg14::lzw_compressor<OutIt, Allocator>& sg14::lzw_compressor<OutIt, Allocator>::operator=(const char& element)
{
	OutIt::operator=(static_cast<char>(element + 1));
	return *this;
}

template<typename OutIt, typename Allocator>
sg14::lzw_decompressor<OutIt, Allocator>::lzw_decompressor(OutIt&& dest_)
	: OutIt(std::move(dest_))
{}

template<typename OutIt, typename Allocator>
sg14::lzw_decompressor<OutIt, Allocator>& sg14::lzw_decompressor<OutIt, Allocator>::operator=(const char& element)
{
	OutIt::operator=(static_cast<char>(element - 1));
	return *this;
}

template<typename OutIt>
sg14::rsa_encryptor<OutIt>::rsa_encryptor(OutIt&& dest_, int m_, int n_)
	: OutIt(std::move(dest_))
	, m(m_)
	, n(n_)
{}

template<typename OutIt>
sg14::rsa_encryptor<OutIt>& sg14::rsa_encryptor<OutIt>::operator=(const char& element)
{
	OutIt::operator=(static_cast<char>(element + m));
	return *this;
}

template<typename OutIt>
sg14::rsa_decryptor<OutIt>::rsa_decryptor(OutIt&& dest_, int m_, int n_)
	: OutIt(std::move(dest_))
	, m(m_)
	, n(n_)
{}

template<typename OutIt>
sg14::rsa_decryptor<OutIt>& sg14::rsa_decryptor<OutIt>::operator=(const char& element)
{
	OutIt::operator=(static_cast<char>(element - m));
	return *this;
}

