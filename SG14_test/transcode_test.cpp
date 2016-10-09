#include "transcode.h"

#include "SG14_test.h"

#include <fstream>
#include <iterator>
#include <algorithm>

void compress_and_encrypt()
{
	std::ifstream input("alice_in_wonderland.txt", std::ios_base::binary);
	std::noskipws(input);
	std::ofstream output("alice_in_cryptoland.txt", std::ios_base::binary | std::ios_base::trunc);
	std::noskipws(output);

	using encryptor = sg14::rsa_encryptor<std::ostream_iterator<char>>;
	using compressor = sg14::lzw_compressor<encryptor>;

	std::copy(std::istream_iterator<char>(input), std::istream_iterator<char>(),
				compressor(encryptor(std::ostream_iterator<char>(output), 27, 91), 16));
}

void decrypt_and_decompress()
{
	std::ifstream input("alice_in_cryptoland.txt");
	std::noskipws(input);
	std::ofstream output("alice_in_plainland.txt");
	std::noskipws(output);

	using decompressor = sg14::lzw_decompressor<std::ostream_iterator<char>>;
	using decryptor = sg14::rsa_decryptor<decompressor>;

	std::copy(std::istream_iterator<char>(input), std::istream_iterator<char>(),
				decryptor(decompressor(std::ostream_iterator<char>(output)), 27, 91));
}

void sg14_test::transcode_test()
{
	compress_and_encrypt();
	decrypt_and_decompress();
}