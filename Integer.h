#include <algorithm>
#include <iostream>
#include <vector>

// 一个简易的高精度整数对象
class Integer {
private:
	// 去除前导零的辅助函数
	void removeLeadingZeros() {
		while (digits.size() > 1 && digits.back() == 0) digits.pop_back();
		if (digits.size() == 1 && digits[0] == 0) isNegative = false;
	}

public:
	std::vector<int> digits; // 存储每一位数字，按小端存储，低位在前
	bool isNegative; // 标记数字是否为负数

	// 默认构造函数，初始化为正数
	Integer() : isNegative(false) {}

	// 从long long类型的整数构造Integer
	Integer(long long number) {
		if (number < 0) {
			isNegative = true;
			number = -number;
		} else isNegative = false;
		if (number == 0) digits.push_back(0);
		else {
			while (number > 0) {
				digits.push_back((int) number % 10); // 将数字分解成个位数并存储
				number /= 10;
			}
		}
	}

	// 从字符串构造Integer
	Integer(const std::string& number) {
		int start = 0;
		if (number[0] == '-') {
			isNegative = true;
			start = 1; // 如果有负号，跳过第一个字符
		} else isNegative = false;
		for (size_t i = start; i < number.size(); ++i) {
			if (isdigit(number[i])) digits.push_back(number[i] - '0'); // 将字符转换成数字并存储
			else throw std::invalid_argument("Invalid digit in number");
		}
		std::reverse(digits.begin(), digits.end()); // 反转以使低位在前
		if (digits.empty()) {
			digits.push_back(0); // 如果输入为空，则设为0
			isNegative = false;
		}
	}

	// 输入运算符重载，用于读取Integer类型的值
	friend std::istream& operator>>(std::istream& in, Integer& Int) {
		Int.digits.clear();
		Int.isNegative = false;
		char ch;
		in >> ch;
		if (ch == '-') {
			Int.isNegative = true;
			in >> ch;
		}
		while (std::isdigit(ch)) {
			Int.digits.push_back(ch - '0'); // 逐个读取字符并转为数字存储
			if (!in.get(ch)) break;
		}
		if (in) in.unget();
		std::reverse(Int.digits.begin(), Int.digits.end());
		Int.removeLeadingZeros(); // 去除前导零
		return in;
	}

	// 输出运算符重载，用于输出Integer类型的值
	friend std::ostream& operator<<(std::ostream& out, const Integer& Int) {
		if (Int.isNegative) out << '-';
		for (auto it = Int.digits.rbegin(); it != Int.digits.rend(); ++it) out << *it; // 逐个输出每一位数字
		return out;
	}

	// 取反运算符重载，返回相反数
	Integer operator-() const {
		Integer result = *this;
		if (*this != 0) result.isNegative = !isNegative; // 如果非零，则切换符号
		return result;
	}

	// 加法运算符重载，实现两个Integer的相加
	Integer operator+(const Integer& other) const {
		if (isNegative == other.isNegative) {
			Integer result;
			result.isNegative = isNegative;
#ifdef max
			const size_t maxLength = max(digits.size(), other.digits.size());
#else
			const size_t maxLength = std::max(digits.size(), other.digits.size());
#endif
			result.digits.resize(maxLength);
			int carry = 0;
			for (size_t i = 0; i < maxLength || carry; ++i) {
				if (i == result.digits.size()) result.digits.push_back(0);
				int currentDigitSum = carry;
				if (i < digits.size()) currentDigitSum += digits[i];
				if (i < other.digits.size()) currentDigitSum += other.digits[i];
				carry = currentDigitSum / 10; // 计算进位
				result.digits[i] = currentDigitSum % 10; // 计算当前位的值
			}
			result.removeLeadingZeros();
			return result;
		}
		if (isNegative) return other - (-(*this)); // 如果符号不同，则转换为减法
		return *this - (-other);
	}

	// 减法运算符重载，实现两个Integer的相减
	Integer operator-(const Integer& other) const {
		if (isNegative != other.isNegative) return *this + (-other); // 如果符号不同，则转换为加法
		if (*this < other) return -(other - *this); // 如果被减数小于减数，则结果为负
		Integer result;
		result.digits.resize(digits.size());
		result.isNegative = isNegative;
		int borrow = 0;
		for (size_t i = 0; i < digits.size(); ++i) {
			int sub = digits[i] - borrow - (i < other.digits.size() ? other.digits[i] : 0);
			if (sub < 0) {
				sub += 10; // 如果不足，则向高位借1
				borrow = 1;
			} else borrow = 0;
			result.digits[i] = sub;
		}
		result.removeLeadingZeros();
		return result;
	}

	// 乘法运算符重载，实现两个Integer的相乘
	Integer operator*(const Integer& other) const {
		Integer result;
		result.isNegative = isNegative != other.isNegative; // 确定结果的符号
		result.digits = std::vector<int>(digits.size() + other.digits.size(), 0); // 初始化结果位数，大小为两个数位数之和
		for (size_t i = 0; i < digits.size(); ++i) {
			long long carry = 0;
			for (size_t j = 0; j < other.digits.size() || carry; ++j) {
				long long current = (long long) result.digits[i + j] + carry + (long long) digits[i] * (j < other.digits.size() ? (long long) other.digits[j] : 0); // 计算当前位的乘积和进位
				result.digits[i + j] = (int) (current % 10); // 取当前位
				carry = (long long) (current / 10); // 计算进位
			}
		}
		result.removeLeadingZeros();
		return result;
	}

	// 除法运算符重载，实现两个Integer的相除
	Integer operator/(const Integer& other) const {
		if (other == 0) throw std::invalid_argument("Division by zero");
		Integer dividend = *this, divisor = other, quotient;
		quotient.isNegative = isNegative != other.isNegative;
		dividend.isNegative = false;
		divisor.isNegative = false;
		if (dividend < divisor) return 0; // 如果被除数小于除数，结果为0
		quotient.digits.resize(dividend.digits.size());
		Integer current;
		for (long long i = (long long) dividend.digits.size() - 1; i >= 0; --i) {
			current.digits.insert(current.digits.begin(), dividend.digits[i]);
			current.removeLeadingZeros();
			int x = 0, left = 0, right = 10;
			while (left <= right) {
				int mid = left + ((right - left) >> 1);
				Integer midVal = divisor * mid;
				if (midVal <= current) {
					x = mid;
					left = mid + 1;
				} else right = mid - 1;
			}
			quotient.digits[i] = x;
			current -= divisor * x;
		}
		quotient.removeLeadingZeros();
		return quotient;
	}

	// 取模运算符重载，实现两个Integer的取模
	Integer operator%(const Integer& other) const {
		if (other == 0) throw std::invalid_argument("Modulo by zero");
		if (other.isNegative) return -(-*this % -other);
		Integer quotient = *this / other;
		if (this->isNegative) --quotient;
		Integer result = quotient * other;
		result.isNegative = false;
		return this->isNegative ? result + *this : *this - result;
	}

	// 幂运算符重载，实现Integer的幂运算
	Integer operator^(const Integer& other) const {
		if (other.isNegative) throw std::invalid_argument("Negative exponent not supported");
		Integer base = *this, exponent = other, result(1);
		while (exponent != 0) {
			if (exponent.digits[0] % 2 == 1) result = result * base; // 如果指数为奇数，乘以基数
			base *= base;
			exponent = exponent / 2; // 指数减半
		}
		return result;
	}

	// 前置自增运算符
	Integer& operator++() { return *this += 1; }

	// 前置自减运算符
	Integer& operator--() { return *this -= 1; }

	// 后置自增运算符
	Integer& operator++(int) { return *this += 1; }

	// 后置自减运算符
	Integer& operator--(int) { return *this -= 1; }

	// 复合加法运算符重载
	Integer& operator+=(const Integer& other) { return *this = *this + other; }

	// 复合减法运算符重载
	Integer& operator-=(const Integer& other) { return *this = *this - other; }

	// 复合乘法运算符重载
	Integer& operator*=(const Integer& other) { return *this = *this * other; }

	// 复合除法运算符重载
	Integer& operator/=(const Integer& other) { return *this = *this / other; }

	// 复合取模运算符重载
	Integer& operator%=(const Integer& other) { return *this = *this % other; }

	// 复合幂运算符重载
	Integer& operator^=(const Integer& other) { return *this = *this ^ other; }

	// 相等运算符重载
	bool operator==(const Integer& other) const { return isNegative == other.isNegative && digits == other.digits; }

	// 不等运算符重载
	bool operator!=(const Integer& other) const { return !(*this == other); }

	// 小于运算符重载
	bool operator<(const Integer& other) const {
		if (isNegative != other.isNegative) return isNegative;
		if (digits.size() != other.digits.size()) return isNegative ? digits.size() > other.digits.size() : digits.size() < other.digits.size();
		for (long long i = (long long) digits.size() - 1; i >= 0; --i) if (digits[i] != other.digits[i]) return isNegative ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
		return false;
	}

	// 小于等于运算符重载
	bool operator<=(const Integer& other) const { return *this < other || *this == other; }

	// 大于运算符重载
	bool operator>(const Integer& other) const { return !(*this <= other); }

	// 大于等于运算符重载
	bool operator>=(const Integer& other) const { return !(*this < other); }

	// 转为字符串
	std::string string() const {
		std::string result;
		if (isNegative) result += '-';
		for (auto it = digits.rbegin(); it != digits.rend(); ++it) result += *it + '0';
		return result;
	}

	// 转为宽字符串
	std::wstring wstring() const {
		std::wstring result;
		if (isNegative) result += '-';
		for (auto it = digits.rbegin(); it != digits.rend(); ++it) result += *it + '0';
		return result;
	}
};

// 粗略计算哈希值
template <>
struct std::hash<Integer> {
	size_t operator()(const Integer& key) const {
		size_t hashValue = 0;
		std::hash<int> intHash;
		for (const int& digit : key.digits) hashValue ^= intHash(digit) + 0x9E3779B9 + (hashValue << 6) + (hashValue >> 2);
		if (key.isNegative) hashValue = ~hashValue;
		return hashValue;
	}
};