
from range import Range
from utils import Fibonacci, read_lines, split


if __name__ == "__main__":

    # Basic test
    print(Range(range(10)).Where(lambda x: x % 2 == 0)  
                          .Select(lambda x: x ** 2)     
                          .Take(2)                      
                          .ToList())
    # Output: [0, 4]

    # Test Fibonacci
    print(Range(Fibonacci()).Where(lambda x: x % 3 == 0)                   
                            .Select(lambda x: x ** 2 if x % 2 == 0 else x) 
                            .Take(5)                                    
                            .ToList())
    # Output: [0, 3, 21, 20736, 987]


    # Test GroupBy
    print(Range(range(10)).GroupBy(lambda x: x % 2)                     
                          .ToList())
    # Output: [(0, [0, 2, 4, 6, 8]), (1, [1, 3, 5, 7, 9])]


    # Test text processing
    print(Range(read_lines("text")).Select(lambda x: split(x.lower()))
                                   .Flatten()
                                   .Count()
                                   .OrderBy(lambda x: -x[1])
                                   .ToList())