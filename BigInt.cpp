#include "BigInt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#pragma warning(disable: 4996)



BigInt::BigInt()
{
	sign = 0;
	elements.push_back(0);
}

BigInt::BigInt(char* str)
{
	sign = 0;
	if(str[0] == '-')
	{
		sign = 1;
	}

	int SizeStr = strlen(str);
	for(int i = SizeStr - 1; i >= sign; i--)
		elements.push_back(str[i] - '0');
	// TODO: i?iaa?ea
}

BigInt::BigInt(int i)
{
   if(i < 0)
	   this->sign = 1;
   else
		this->sign = 0;

   this->elements.push_back(abs(i) % BASE);
   i /= BASE;

   if(i != 0)
	   this->elements.push_back(abs(i));
}

BigInt::~BigInt()
{
	elements.clear();
}

char* BigInt::getString()
{
	char* str = new char [elements.size() + 1 + sign]();
	int j = elements.size() + sign - 1;
	for(size_t i = 0; i < elements.size(); i++)
	{
		str[j] = elements[i] + '0';
		j--;
	}
	if(sign == 1)
		str[0] = '-';

	return str;
}

int cmp(BigInt A, BigInt B)
{ 
	int sign_A = 1;
	if(A.sign == 1)
		sign_A = -1;

	if(A.sign != B.sign)
		return sign_A;

	if (A.elements.size() > B.elements.size())
		return sign_A;

	if (A.elements.size() < B.elements.size())
		return -sign_A;

	int	i = A.elements.size() - 1;

	while( A.elements[i] ==  B.elements[i] && i > 0)
	{
		i--;
	}
	return  (A.elements[i] -  B.elements[i]) * sign_A;


	return 0;
}

bool  BigInt::getFrom_txt(const char* filename)
{
  FILE *fp = fopen(filename,"r");
  if (!fp)
		return false;
  fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char * str = new char[fileSize + 1]();
  for(int i = 0; i < fileSize ; i++)
  {
	  str[i] = fgetc(fp) ;
  }

  fclose(fp);
  BigInt number(str);
  *this = number;

  delete[]str;

  return true;

}

bool BigInt::saveTo_txt(const char* filename)
{
 FILE *fp = fopen(filename,"w");
  if (!fp)
		return false;

  char* str = this->getString();
  int len = strlen(str);
  fwrite(str, sizeof(char), len, fp);
  fclose (fp);

  delete[] str;

  return true;

}

bool BigInt::getFrom_bin(const char* filename)
{
   FILE *fp = fopen(filename,"r+b");				 //ia aoia ?enei a aeaa 0X000000
  if (!fp)											 //ciae eee 1- "-" eee 0
		return false;								 //o.a "- "ciae aoaao a aeaa 01000000...
   fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  BigInt number;

   fread(&number.sign, sizeof(number.sign),1,fp);
   	fileSize  /= sizeof(int);
	number.elements.resize(fileSize - 1);
	for(size_t i = 0 ; i < fileSize - 1 ; i ++)
	{
		fread(&number.elements[number.elements.size()- 1 - i], sizeof(int),1,fp);
	}

	*this = number;

  fclose (fp);
  return true;
}

bool BigInt::saveTo_bin(const char* filename)
{												  //ia auoiaa ?enei a aeaa 0X000000
   FILE *fp = fopen(filename,"w+b");
  if (!fp)
		return false;
  BigInt number = *this;

  fwrite(&number.sign,sizeof(number.sign), 1,fp);
  for(size_t i = 0 ; i < number.elements.size() ; i ++)
  {
	  fwrite(&number.elements[ number.elements.size() - 1 - i],sizeof(int), 1,fp);
  }

  fclose (fp);
  return true;
}

bool BigInt::operator>(const BigInt& B)
{
	return cmp(*this, B) > 0;
}

bool BigInt::operator<(const BigInt& B)
{
	return cmp(*this, B) < 0;
}

bool BigInt::operator<=(const BigInt& B)
{
	return cmp(*this, B) <= 0;
}

bool BigInt::operator>=(const BigInt& B)
{
	return cmp(*this, B) >= 0;
}

bool BigInt::operator==(const BigInt& B)
{
	return cmp(*this, B) == 0;
}

bool BigInt::operator!=(const BigInt& B)
{
	return cmp(*this, B) != 0;
}

BigInt BigInt::operator + (const BigInt& B) const
{
	BigInt A = *this;

	if (A.sign)
	{
		if (B.sign)
			return -(-A + (-B));
		else
			return B - (-A);
	}
	else if (B.sign)
		return A - (-B);

	int carry = 0; 

	for (size_t i = 0; i < max(A.elements.size(), B.elements.size()) || carry != 0; i++)
	{
		if (i == A.elements.size())
			A.elements.push_back(0);

		A.elements[i] += carry +  (i < B.elements.size() ? B.elements[i] : 0);

		carry = A.elements[i] >= BASE;

		if (carry != 0)
			A.elements[i] -= BASE;
	}

	return A;
}

BigInt BigInt::operator - (const BigInt& B) const
{
	BigInt A = *this;

	if (B.sign)
		return A + (-B);
	else if (A.sign)
		return -(-A + B);
	else if (A < B)
		return -(B - A);

	int carry = 0; 

	for (size_t i = 0; i < B.elements.size() || carry != 0; i++)
	{
		A.elements[i] -= carry +  (i < B.elements.size() ? B.elements[i] : 0);

		carry = A.elements[i] < 0;

		if (carry != 0)
			A.elements[i] += BASE;
	}
	A.DelZeros();
	return A;

}

BigInt BigInt::operator * (const BigInt& B) const
{
	BigInt A = *this;
	BigInt C;
	size_t temp = 0, carry = 0;
	C.elements.resize( A.elements.size() + B.elements.size());
	for(size_t i = 0; i < A.elements.size(); i++)
	{
		carry = 0;
		for(size_t j = 0; j < B.elements.size(); j++)
		{
			temp = A.elements[i] * B.elements[j] + C.elements[i+j] + carry; 
			carry  = temp / BASE;
			C.elements[i + j] = temp % BASE;
		}
		C.elements[i + B.elements.size()] = carry;
	}
	 C.sign = A.sign!=B.sign;

	C.DelZeros();

	return C;
}

BigInt BigInt::operator / (const BigInt& B) const
{
	BigInt A = *this;
	BigInt b = B;
	BigInt C, current;
	
	if(b == 0)
	{
		throw DEV_BY_ZERO;
		return BigInt(-1);
	}
	 
	C.elements.resize(A.elements.size());
	for(long long i = (A.elements.size() - 1); i >= 0; i --)
	{
		current.ShiftRight();
		current.elements[0] = A.elements[i];
		current.DelZeros();

		int x = 0, l = 0, m = 0, r = BASE;

		
		while(l <=r)
		{
			m = (l + r) / 2;
			BigInt t = b * m;

			if(t <= current)
			{
			  x = m;
			  l = m + 1;
			}
			else
				r = m -1;
		}
		C.elements[i] = x;
		current = current - b * x;
	}
	C.sign = A.sign != B.sign;

	C.DelZeros();

	return C;
}

BigInt BigInt::operator%(const BigInt& B)const
{
	BigInt C = *this - (*this / B) * B;

	if(C.sign)
		C += B;
	return C;
}

BigInt BigInt::operator ^ (const BigInt& B) const
{
	BigInt a = *this, b = B, c = 1;

	while(b != 0)
	{
	   c *= a;
	   b--;
	}
	return c;
}

void BigInt::DelZeros()
{
	while (this->elements.size() > 1 && this->elements.back() == 0)
		this->elements.pop_back();
	if(this->elements.size() == 1 && this->elements[0] == 0)
		this->sign = 0;


}

void BigInt::ShiftRight()
{
	if(this->elements.size() == 0)
	{
		this->elements.push_back(0);
		return;
	}

	this->elements.push_back(this->elements[this->elements.size() - 1]);
	for(size_t i = this->elements.size() - 2; i > 0; i--)
		this->elements[i] = this->elements[i - 1];
	this->elements[0] = 0;
}

BigInt BigInt::operator +=(const BigInt& B)
{
	return *this = *this + B;
}

BigInt BigInt::operator -=(const BigInt& B)
{
	return *this = *this - B;
}

BigInt BigInt::operator *=(const BigInt& B)
{
	return *this = *this * B;
}

BigInt BigInt::operator /=(const BigInt& B)
{
	return *this = *this / B;
}

BigInt BigInt::operator %=(const BigInt& B)
{
	return *this = *this % B;
}

BigInt BigInt::operator ^=(const BigInt& B)
{
	return *this = *this ^ B;
}

BigInt BigInt::operator -() const
{
	BigInt temp = *this;

	temp.sign = !temp.sign;

	return temp;
}

BigInt BigInt::operator ++(int)
{
	BigInt old = *this;
	*this = *this + 1;
	return old;
}

BigInt BigInt::operator ++()
{
	*this = *this + 1;
	return *this;
}

BigInt BigInt::operator --(int)
{
	BigInt old = *this;
	*this = *this - 1;
	return old;
}

BigInt BigInt::operator --()
{
	*this = *this - 1;
	return *this;
}
char * BigInt::__str__()
{
		return getString();
}

char * BigInt::__repr__()
{
		return getString();
}
bool BigInt::operator>(const long long int B)
{
	return cmp(*this, B) > 0;
}

bool BigInt::operator<(const long long int B)
{
	return cmp(*this, B) < 0;
}

bool BigInt::operator<=(const long long int B)
{
	return cmp(*this, B) <= 0;
}

bool BigInt::operator>=(const long long int B)
{
	return cmp(*this, B) >= 0;
}

bool BigInt::operator==(const long long int B)
{
	return cmp(*this, B) == 0;
}

bool BigInt::operator!=(const long long int B)
{
	return cmp(*this, B) != 0;
}

BigInt BigInt::operator +=(const long long int B)
{
	return *this = *this + (BigInt) B;
}

BigInt BigInt::operator -=(const long long int B)
{
	return *this = *this - (BigInt) B;
}

BigInt BigInt::operator *=(const long long int B)
{
	return *this = *this * (BigInt) B;
}

BigInt BigInt::operator /=(const long long int B)
{
	return *this = *this / (BigInt) B;
}

BigInt BigInt::operator %=(const long long int B)
{
	return *this = *this % (BigInt) B;
}

BigInt BigInt::operator ^=(const long long int B)
{
	return *this = *this ^ (BigInt) B;
}

BigInt BigInt::operator+(const long long int B)const
{
	return *this + (BigInt) B;
}
BigInt BigInt::operator-(const long long int B)const
{
	return *this - (BigInt) B;
}
BigInt BigInt::operator*(const long long int B)const
{
	return *this * (BigInt) B;
}
BigInt BigInt::operator/(const long long int B)const
{
	return *this / (BigInt) B;
}
BigInt BigInt::operator%(const long long int B)const
{
	return *this % (BigInt) B;
}
BigInt BigInt::operator^(const long long int B)const
{
	return *this ^ (BigInt) B;
}
BigInt BigInt::powmod(BigInt& a, BigInt& k, BigInt& n)
{
    BigInt res(1), aa = a, kk = k;

    while (kk != 0)
    {
        if ((kk % 2) == 1)
			res = (res * aa) % n;
        
		aa = (aa * aa) % n;

        kk /= 2;
    }

    return res;
}
BigInt GenerateRandomLen(int bitLen)
{
    BigInt res(1);
    size_t res_size ;
    res_size = bitLen / 8;
    res.elements.resize(res_size);

    for (size_t i = 0; i < res_size; i++)
    {
        res.elements[i] = rand() % BASE;
        if (i == res_size - 1)
        {
            res.elements[i] = rand() % (BASE - 1) + 1;
        }
    }
    return res;
}


BigInt GenerateRandomMax(BigInt max)
{
    BigInt res(1);
    do
    {

        size_t res_size ;
        res_size= max.elements.size();
        res.elements.resize(res_size);

        for (int i = 0; i < res_size; i++)
        {
            res.elements[i] = rand() % BASE;
            if (i == res_size - 1)
            {
                res.elements[i] = rand() % (max.elements[i] + 1);
            }
        }
    } while (res > max);
    
    res.DelZeros();
    return res;
}

void initRandom()
{
    srand(time(NULL));
}