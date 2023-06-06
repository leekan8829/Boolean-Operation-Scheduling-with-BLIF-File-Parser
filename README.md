# Boolean Operation Scheduling with BLIF File Parser
Run-time Example:  
%> list -l sample02.blif 2 1 1  
Resource-constrained Scheduling  
1: {h} {j} {i}  
2: {l m} {g} {}  
3: {n} {k} {p}  
4: {o} {} {q}  
#AND: 2  
#OR: 1  
#NOT: 1  
END  
  
  
Run-time Example:  
%> list -r sample02.blif 5  
Latency-constrained Scheduling  
1: {h} {j} {i}  
2: {m} {} {}  
3: {l} {g} {}  
4: {n} {k} {p}  
5: {o} {} {q}  
#AND: 1  
#OR: 1  
#NOT: 1  
END  
  
Run-time Example:  
%> list -r sample02.blif 3  
Latency-constrained Scheduling  
No feasible solution.  
END  
