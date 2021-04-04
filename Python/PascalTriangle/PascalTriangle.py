def pascal(n):
	if n==0 or n==1:
		return [1]*(n+1)
	else:
		p = pascal(n-1)
		return [1] + [p[e] + p[e+1] for e in range(len(p)-1)] + [1]

print(pascal(7))
