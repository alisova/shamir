# coding: UTF-8

import random, BigInt


def GeneratePrime(bitLen):
    p = BigInt.GenerateRandomLen(bitLen)
    while not MillerRabin(p):
        p += 1
    return p

	
# тест Миллера-Рабина
def MillerRabin(m):
    m = m.getString()
    m = int(m)
    t = m - 1
    s = BigInt.BigInt(0)
    while t % 2 == 0:
        t /= 2
        s += 1
            
    for i in range(20):
        #a = BigInt.GenerateRandomMax(m-4)+2
        a = random.randint(1, m-1)
        x = pow(a,t,m)#m.powmod(a, t, m)
        if x == 1:
            return True # составное
    i = BigInt.BigInt(0)
    while i < s - 1:
        x = (x * x) % m
        if x == m - 1:
            return True
        i = i + 1
    return x == m - 1
				
		
# генерация ключей		
def KeyGen(bitlen):
   #3-е лицо генерирует простые  p,q и вычисляет n
    p = GeneratePrime(bitlen)
    q = GeneratePrime(bitlen)
    n = p*q
    # Алиса генерирует s ивычисляет v
    s = BigInt.GenerateRandomMax(n - 1)
    two = BigInt.BigInt(2)
    v = n.powmod(s,two,n)
    pub_key = "{}\n{}".format(n, v)
    priv_key = "{}\n{}\n{}".format(s, p, q)

    return pub_key, priv_key, n, s, v

	
def main():
    print "Fiat-Shamir"
    bitlen = 1024
    # Предварительные действия		
    pub_key, priv_key, n, s, v = KeyGen(bitlen)

    # Передаваемые сообщения (этапы каждой аккредитации)
    # 1 A -> B : x = r^2(mod n)
    r = BigInt.GenerateRandomMax(n - 1)
    two = BigInt.BigInt(2)
    x = n.powmod(r,two,n)
    # 2 A <- B : e\in{0,1}
    ee = random.randint(0, 1)
    e = BigInt.BigInt(ee)
    print "e ", e
    # 3 A -> B : y = r * s^e(mod n)
    if e == 1:
        y = (r * n.powmod(s,e,n) % n)
    else:
        y = (r % n )
    
    # 4 Алиса передает ответ Бобу, чтобы показать, что она знает значение своего секретного ключа, s. 
	
    # 5 Боб вычисляет y^2 и x^e. 
    if e == 1:
        y = (x*v)%n
    else:
        y = x % n #n.powmod(x,two,n)

    yy = (x * n.powmod(v,e,n) % n) 

    if y == yy:
         print "Accepted"
         print "y^2 by A = by B"
    else:
        print "Reject"
        print "y^2 by A != yy by B"

    return y, yy
    		
			
if __name__ == "__main__":
    main()	