#include "Util.h"

#include <sstream>
#include <format>

namespace Util {
	std::string hex(int x) {
		return std::format("{:#x}", x);
	}
}