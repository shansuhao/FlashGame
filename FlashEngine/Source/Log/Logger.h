#pragma once

class Logger {
private:
	Logger() = default;
public:
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) const = delete;

	inline static Logger& Get() {
		static Logger self;
		return self;
	}

private:
	int  success;
	char infoLog[512];

public:
	void PrintLog(unsigned int target, unsigned int type);
};