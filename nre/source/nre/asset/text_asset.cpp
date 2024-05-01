#include <nre/asset/text_asset.hpp>



namespace nre {

	F_text_asset::F_text_asset() {

	}
	F_text_asset::~F_text_asset() {

	}

	void F_text_asset::build(const F_asset_buffer& buffer) {

		content = str_from_buffer(buffer);
	}

	G_wstring F_text_asset::str_from_buffer(const F_asset_buffer& buffer) {

		G_wstring out;
		unsigned int codepoint;

		auto buffer_end = buffer.cend();
		auto* in = buffer.cbegin();

		while (in != buffer_end)
		{
			unsigned char ch = static_cast<unsigned char>(*in);
			if (ch <= 0x7f)
				codepoint = ch;
			else if (ch <= 0xbf)
				codepoint = (codepoint << 6) | (ch & 0x3f);
			else if (ch <= 0xdf)
				codepoint = ch & 0x1f;
			else if (ch <= 0xef)
				codepoint = ch & 0x0f;
			else
				codepoint = ch & 0x07;
			++in;
			if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
			{
				if (sizeof(wchar_t) > 2)
					out.append(1, static_cast<wchar_t>(codepoint));
				else if (codepoint > 0xffff)
				{
					codepoint -= 0x10000;
					out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
					out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
				}
				else if (codepoint < 0xd800 || codepoint >= 0xe000)
					out.append(1, static_cast<wchar_t>(codepoint));
			}
		}

		return std::move(out);
	}

}