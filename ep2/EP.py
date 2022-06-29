from sage.all import *

from random import *

from interval import interval

import numpy as np
import math
import hashlib
# http://oliviertech.com/python/generate-SHA512-hash-from-a-String/

q = 271

F = Zmod (q)

curve = EllipticCurve (F, [0, 0, 0, -7, 0])

NUSP = 9922700

def hash_ep (x):
    return int (hashlib.sha512 (x.encode("utf-8")).hexdigest())

# https://crypto.stackexchange.com/questions/66678/how-to-find-the-generators-of-an-elliptic-curve
def checkP (P = curve ([201, 247])):
	if P in curve:
        print ("P is in the curve")

        # order of curve 
        m = curve.order ()

    #    if m * P = 0:
     #       print ("P is not generator of the elliptic curve")
      #      return false

     #   for i in divisors (m) [:-1]:
       #     if i * P == 0:
   #             print ("P is not generator of the elliptic curve")
      #          return false
        if P.order () > 1:
            print ("P is not generator of the elliptic curve")
            return false

		return true
	else
		print ("P is not in the curve")
        return false

def checkR (R = curve ([177, 147])):
    return R in curve

def signature (q, curve, x, (P, Q)):
    n = P.order ()
    r = 0, a = 0

    while (a == 0):
        while (r == 0):
            # 1 - NONCE
            k = randint (1, n - 1)

            # 2 - kP = (x1, x2) and 
            #     r = x1 mod n
            kP = k * P

            r = kP[0] % (n)

        # 3 - inverse of k mod n
        k_inv = inverse_mod (k, n)

        # 4 - a 
        a = k_inv * (hash_ep (x) + s * r) % n
    
    return (r, a)

def verify (x, (r, a), (P, Q)):
    n = P.order ()

    if !(r in interval [1, n - 1]) or !(a in interval [1, n - 1]):
        print ("Signature rejected 'a' or 'r' are not in the interval")
        return false

    w = inverse_mod (a, n)

    u1 = (w * (hash_ep (x))) % n

    u2 = (w * r) % n

    res = u1 * P + u2 * Q

    v = res [0] % n

    if v == r:
        return true

    return false


def main ():
    # 2 - Check P in elliptic curve
    P = input ("Digite P:")

    if !checkP (P):
        print ("Erro P não pertence a curva")

    # 3 - Check Q in elliptic curve
    R = input ("Digite R:")

    if !checkR (R):
        print ("Erro R não pertence a curva")

    # 4 - Sum P + R in the elliptic curve and print
    PR = P + R

    print ("P + R: ", PR)

    # 5 - calculate s (random number in a real crypto program)
    s = NUSP % 271

    while s == 0:
        NUSP += 1
        s = NUSP % 271

    print ("s: ", s)

    # 6 - Calculate Q = sP and show
    Q = s * P

    print ("sP = Q: ", Q)

    # 7 - sign x with Menezes-Vanstone algorithm
    x = 214

    (r, a) = signature (x, q, curve, n, s, (P, Q))

    print ("(r, a): ", (r, a))

    # 8 - verify signature and show result
    res = verify (x, (r, a), (P, Q))

    print ("Verification: ", res)







