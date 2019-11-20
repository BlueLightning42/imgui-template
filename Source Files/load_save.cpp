#include "pch.h"

#include "load_save.h"
#include "graphics.h" //for check_result


/* Note to self. *

Either I store config information in a editable format and in that case I really want to use hjson
or I store it in a serialized binary format for "super fast" reading and writing.

I dont really see an in between and I'm leaning towards binary for now so open times are fast.

another option is to use fmt which I already have in this project to format to a memory buffer and serialize it into txt
*/
// static cast is to silence warning
static void serialize_u32(byte* buf, int32_t val, int &index) {
	uint32_t uval = val;
	buf[index + 0] = static_cast<byte>(uval		);
	buf[index + 1] = static_cast<byte>(uval >> 8);
	buf[index + 2] = static_cast<byte>(uval >> 16);
	buf[index + 3] = static_cast<byte>(uval >> 24);
	index += 4;
}

static uint32_t parse_u32(byte* buf, int &index) {
	// This prevents buf[i] from being promoted to a signed int.
	uint32_t u0 = buf[index], u1 = buf[index+1], u2 = buf[index+2], u3 = buf[index+3];
	uint32_t uval = u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);
	index += 4;
	return uval;
}
static void serialize_u16(byte* buf, int32_t val, int &index) {
	uint32_t uval = val;
	buf[index + 0] = static_cast<byte>(uval		);
	buf[index + 1] = static_cast<byte>(uval >> 8);
	index += 2;
}

static uint16_t parse_u16(byte* buf, int &index) {
	// This prevents buf[i] from being promoted to a signed int.
	uint16_t u0 = buf[index], u1 = buf[index+1];
	uint16_t uval = u0 | (u1 << 8);
	//buf += 2;
	index += 2;
	return uval;
}
static void serialize_u8(byte* buf, int8_t val, int &index) {
	uint32_t uval = val;
	buf[index + 0] = static_cast<byte>(uval);
	index += 1;
}

static uint8_t parse_u8(byte* buf, int &index) {
	// This prevents buf[i] from being promoted to a signed int.
	uint8_t u0 = buf[index];
	//buf++;
	index += 1;
	return u0;
}


void GraphicContext::load_user_info() {
	// open config file.
	// read user settings and fill user_info struct


	std::ifstream in("config.bin", std::ios::binary);
	if (in.is_open()) {
		byte buffer[UserInfo::size +1];
		in.seekg(0, std::ios::end);
		if (in.tellg() == UserInfo::size) {
			in.seekg(0, std::ios::beg);
			in.read((char*)buffer, UserInfo::size);
			
			int index = 0;
			settings.fullscreen = parse_u8(buffer, index);
			settings.win_width =  parse_u32(buffer, index);
			settings.win_height = parse_u32(buffer, index);

#if defined(_DEBUG)
			fmt::print("Settings loaded:  \n\tfullscreen: {}\n\twidth: {}\n\theight: {}\n",
				settings.fullscreen, settings.win_width, settings.win_height);
#endif // DEBUG
			return;
		}
	}
	// defaults/ if failed
	settings.fullscreen = false;
	settings.win_width = 1250;
	settings.win_height = 720;

	return;
}	

void GraphicContext::store_user_info() {
	//byte* buffer = (byte*)(std::malloc(UserInfo::size * sizeof(byte) + 1));

	byte buffer[UserInfo::size];
	int index = 0;

	serialize_u8(buffer, settings.fullscreen, index);
	serialize_u32(buffer, settings.win_width, index);
	serialize_u32(buffer, settings.win_height, index);

	//note trashes buffer if this fails but its fine its already crashing. (buffer isn't freed)
	check_result(index == UserInfo::size, "Didn't write everything to buffer when storing user info.");

	std::ofstream out("config.bin", std::ios::binary);
	out.write((const char*)buffer, UserInfo::size);
	//std::free(buffer);
}
