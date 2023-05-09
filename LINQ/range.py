from functools import cmp_to_key
import collections


class Range:

    def __init__(self, data):
        self.iterable = data

    def Where(self, cond):
        def innerWhere(obj, cond):
            for elem in obj:
                if cond(elem):
                    yield elem
        self.iterable = innerWhere(self.iterable, cond)
        return self
    
    def Select(self, mod):
        def innerSelect(obj, func):
            for elem in obj:
                yield func(elem)
        self.iterable = innerSelect(self.iterable, mod)
        return self
    
    def Take(self, lim):
        def innerTake(obj, limit):
            for i, elem in enumerate(obj):
                if i == limit:
                    break
                yield elem
        self.iterable = innerTake(self.iterable, lim)
        return self

    def ToList(self):
        return list(self.iterable)
    
    def OrderBy(self, comparator = lambda x: x):
        self.iterable = sorted(self.iterable, key=comparator)
        return self

    def Count(self):
        counted = collections.Counter(self.iterable)
        def glide(obj):
            for elem in obj.items():
                yield elem
        self.iterable = glide(counted)
        return self

    def Flatten(self):
        def glide(obj):
            for elem in obj:
                if hasattr(elem, '__iter__'):
                    for subelem in elem:
                        yield subelem
                else:
                    yield elem
        self.iterable = glide(self.iterable)
        return self
    
    def GroupBy(self, build_key):
        mapping = dict()
        for elem in self.iterable:
            key = build_key(elem)
            if key in mapping:
                mapping[key].append(elem)
            else:
                mapping[key] = [elem]
        def glide(obj):
            for elem in obj:
                yield elem
        self.iterable = glide(mapping.items())
        return self
