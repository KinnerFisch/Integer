#include <algorithm>
#include <iostream>
#include <vector>

// һ�����׵ĸ߾�����������
class Integer {
private:
	// ȥ��ǰ����ĸ�������
	void removeLeadingZeros() {
		while (digits.size() > 1 && digits.back() == 0) digits.pop_back();
		if (digits.size() == 1 && digits[0] == 0) isNegative = false;
	}

public:
	std::vector<int> digits; // �洢ÿһλ���֣���С�˴洢����λ��ǰ
	bool isNegative; // ��������Ƿ�Ϊ����

	// Ĭ�Ϲ��캯������ʼ��Ϊ����
	Integer() : isNegative(false) {}

	// ��long long���͵���������Integer
	Integer(long long number) {
		if (number < 0) {
			isNegative = true;
			number = -number;
		} else isNegative = false;
		if (number == 0) digits.push_back(0);
		else {
			while (number > 0) {
				digits.push_back((int) number % 10); // �����ַֽ�ɸ�λ�����洢
				number /= 10;
			}
		}
	}

	// ���ַ�������Integer
	Integer(const std::string& number) {
		int start = 0;
		if (number[0] == '-') {
			isNegative = true;
			start = 1; // ����и��ţ�������һ���ַ�
		} else isNegative = false;
		for (size_t i = start; i < number.size(); ++i) {
			if (isdigit(number[i])) digits.push_back(number[i] - '0'); // ���ַ�ת�������ֲ��洢
			else throw std::invalid_argument("Invalid digit in number");
		}
		std::reverse(digits.begin(), digits.end()); // ��ת��ʹ��λ��ǰ
		if (digits.empty()) {
			digits.push_back(0); // �������Ϊ�գ�����Ϊ0
			isNegative = false;
		}
	}

	// ������������أ����ڶ�ȡInteger���͵�ֵ
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
			Int.digits.push_back(ch - '0'); // �����ȡ�ַ���תΪ���ִ洢
			if (!in.get(ch)) break;
		}
		if (in) in.unget();
		std::reverse(Int.digits.begin(), Int.digits.end());
		Int.removeLeadingZeros(); // ȥ��ǰ����
		return in;
	}

	// �����������أ��������Integer���͵�ֵ
	friend std::ostream& operator<<(std::ostream& out, const Integer& Int) {
		if (Int.isNegative) out << '-';
		for (auto it = Int.digits.rbegin(); it != Int.digits.rend(); ++it) out << *it; // ������ÿһλ����
		return out;
	}

	// ȡ����������أ������෴��
	Integer operator-() const {
		Integer result = *this;
		if (*this != 0) result.isNegative = !isNegative; // ������㣬���л�����
		return result;
	}

	// �ӷ���������أ�ʵ������Integer�����
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
				carry = currentDigitSum / 10; // �����λ
				result.digits[i] = currentDigitSum % 10; // ���㵱ǰλ��ֵ
			}
			result.removeLeadingZeros();
			return result;
		}
		if (isNegative) return other - (-(*this)); // ������Ų�ͬ����ת��Ϊ����
		return *this - (-other);
	}

	// ������������أ�ʵ������Integer�����
	Integer operator-(const Integer& other) const {
		if (isNegative != other.isNegative) return *this + (-other); // ������Ų�ͬ����ת��Ϊ�ӷ�
		if (*this < other) return -(other - *this); // ���������С�ڼ���������Ϊ��
		Integer result;
		result.digits.resize(digits.size());
		result.isNegative = isNegative;
		int borrow = 0;
		for (size_t i = 0; i < digits.size(); ++i) {
			int sub = digits[i] - borrow - (i < other.digits.size() ? other.digits[i] : 0);
			if (sub < 0) {
				sub += 10; // ������㣬�����λ��1
				borrow = 1;
			} else borrow = 0;
			result.digits[i] = sub;
		}
		result.removeLeadingZeros();
		return result;
	}

	// �˷���������أ�ʵ������Integer�����
	Integer operator*(const Integer& other) const {
		Integer result;
		result.isNegative = isNegative != other.isNegative; // ȷ������ķ���
		result.digits = std::vector<int>(digits.size() + other.digits.size(), 0); // ��ʼ�����λ������СΪ������λ��֮��
		for (size_t i = 0; i < digits.size(); ++i) {
			long long carry = 0;
			for (size_t j = 0; j < other.digits.size() || carry; ++j) {
				long long current = (long long) result.digits[i + j] + carry + (long long) digits[i] * (j < other.digits.size() ? (long long) other.digits[j] : 0); // ���㵱ǰλ�ĳ˻��ͽ�λ
				result.digits[i + j] = (int) (current % 10); // ȡ��ǰλ
				carry = (long long) (current / 10); // �����λ
			}
		}
		result.removeLeadingZeros();
		return result;
	}

	// ������������أ�ʵ������Integer�����
	Integer operator/(const Integer& other) const {
		if (other == 0) throw std::invalid_argument("Division by zero");
		Integer dividend = *this, divisor = other, quotient;
		quotient.isNegative = isNegative != other.isNegative;
		dividend.isNegative = false;
		divisor.isNegative = false;
		if (dividend < divisor) return 0; // ���������С�ڳ��������Ϊ0
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

	// ȡģ��������أ�ʵ������Integer��ȡģ
	Integer operator%(const Integer& other) const {
		if (other == 0) throw std::invalid_argument("Modulo by zero");
		if (other.isNegative) return -(-*this % -other);
		Integer quotient = *this / other;
		if (this->isNegative) --quotient;
		Integer result = quotient * other;
		result.isNegative = false;
		return this->isNegative ? result + *this : *this - result;
	}

	// ����������أ�ʵ��Integer��������
	Integer operator^(const Integer& other) const {
		if (other.isNegative) throw std::invalid_argument("Negative exponent not supported");
		Integer base = *this, exponent = other, result(1);
		while (exponent != 0) {
			if (exponent.digits[0] % 2 == 1) result = result * base; // ���ָ��Ϊ���������Ի���
			base *= base;
			exponent = exponent / 2; // ָ������
		}
		return result;
	}

	// ǰ�����������
	Integer& operator++() { return *this += 1; }

	// ǰ���Լ������
	Integer& operator--() { return *this -= 1; }

	// �������������
	Integer& operator++(int) { return *this += 1; }

	// �����Լ������
	Integer& operator--(int) { return *this -= 1; }

	// ���ϼӷ����������
	Integer& operator+=(const Integer& other) { return *this = *this + other; }

	// ���ϼ������������
	Integer& operator-=(const Integer& other) { return *this = *this - other; }

	// ���ϳ˷����������
	Integer& operator*=(const Integer& other) { return *this = *this * other; }

	// ���ϳ������������
	Integer& operator/=(const Integer& other) { return *this = *this / other; }

	// ����ȡģ���������
	Integer& operator%=(const Integer& other) { return *this = *this % other; }

	// ���������������
	Integer& operator^=(const Integer& other) { return *this = *this ^ other; }

	// ������������
	bool operator==(const Integer& other) const { return isNegative == other.isNegative && digits == other.digits; }

	// �������������
	bool operator!=(const Integer& other) const { return !(*this == other); }

	// С�����������
	bool operator<(const Integer& other) const {
		if (isNegative != other.isNegative) return isNegative;
		if (digits.size() != other.digits.size()) return isNegative ? digits.size() > other.digits.size() : digits.size() < other.digits.size();
		for (long long i = (long long) digits.size() - 1; i >= 0; --i) if (digits[i] != other.digits[i]) return isNegative ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
		return false;
	}

	// С�ڵ������������
	bool operator<=(const Integer& other) const { return *this < other || *this == other; }

	// �������������
	bool operator>(const Integer& other) const { return !(*this <= other); }

	// ���ڵ������������
	bool operator>=(const Integer& other) const { return !(*this < other); }

	// תΪ�ַ���
	std::string string() const {
		std::string result;
		if (isNegative) result += '-';
		for (auto it = digits.rbegin(); it != digits.rend(); ++it) result += *it + '0';
		return result;
	}

	// תΪ���ַ���
	std::wstring wstring() const {
		std::wstring result;
		if (isNegative) result += '-';
		for (auto it = digits.rbegin(); it != digits.rend(); ++it) result += *it + '0';
		return result;
	}
};

// ���Լ����ϣֵ
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