#include "Encryption.h"
#include "md5.h"
#include "sha512.hh"

std::string hashPassword(std::string password, std::string encryption){
	if (encryption == "md5") {
		return md5(password);
	}
	else {
		return	sw::sha512::calculate(password);
	}
}

std::string hashPassword(std::string password) {
	return hashPassword(password, "sha512");
}