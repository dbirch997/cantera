import sys
cdef int _pythonMajorVersion = sys.version_info[0]

cdef string stringify(x):
    """ Converts Python strings to std::string. """
    # This method works with both Python 2.x and 3.x.
    tmp = bytes(x.encode())
    return string(tmp)

cdef pystr(string x):
    cdef bytes s = x.c_str()
    if _pythonMajorVersion == 2:
        # Python 2.x
        return s
    else:
        # Python 3.x
        return s.decode()

def addDirectory(directory):
    CxxAddDirectory(stringify(directory))
