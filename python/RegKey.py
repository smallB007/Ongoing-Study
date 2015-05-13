# -*- coding: utf-8 -*-

# 2014-10-29T19:30+08:00
# By myd7349(myd7349@gmail.com)

# TODO: documentation and unit test

import os.path
import winreg

# These 2 functions below are part of the internal implementation,
# and you should not call them directly.
def _OpenKeyToEnum(key, sub_key):
    if sub_key != None and not isinstance(sub_key, str):
        raise TypeError('argument "sub_key" must be str or None')
    
    if sub_key != None and sub_key != '':
        return winreg.OpenKeyEx(key, sub_key, 0, winreg.KEY_READ)
    else:
        return key
def _CloseKeyToEnum(key, key_to_enum):
    if key_to_enum and key_to_enum != key:
        winreg.CloseKey(key_to_enum)

_Join = os.path.join

def _EnumSubKeyImpl(key, sub_key = None, recursive = False, first_recursion = True):
    key_to_enum = _OpenKeyToEnum(key, sub_key)
    sub_key_cnt, val_cnt, last_modified = winreg.QueryInfoKey(key_to_enum)

    if recursive:
        if first_recursion:
            for i in range(sub_key_cnt):
                yield from _EnumSubKeyImpl(key_to_enum,
                                           winreg.EnumKey(key_to_enum, i),
                                           recursive, False)
        else:
            if sub_key_cnt == 0:
                yield sub_key
            else:
                for i in range(sub_key_cnt):
                    new_sub_key = _Join(sub_key, winreg.EnumKey(key_to_enum, i))
                    yield from _EnumSubKeyImpl(key, new_sub_key, recursive, False)
    else:
        for i in range(sub_key_cnt):
            yield winreg.EnumKey(key_to_enum, i)

    _CloseKeyToEnum(key, key_to_enum)

# The old version of EnumSubKey looks like this:
#EnumSubKey = functools.update_wrapper(
#    functools.partial(_EnumSubKeyImpl, first_recursion = True), _EnumSubKeyImpl)
# in which case 'help(EnumSubKey)' will not work but 'print(EnumSubKey.__doc__)' works.
# So I may need some black-magic:
# http://stackoverflow.com/questions/16672856/allow-help-to-work-on-partial-function-object
# But it is even more confusing. So I rewrite it.
def EnumSubKey(key, sub_key = None, recursive = False):
    '''Enumerate all sub-keys under specified registry item.'''
    return _EnumSubKeyImpl(key, sub_key, recursive, True) # See issue #1

def EnumValue(key, sub_key = None, recursive = False):
    '''Enumerate all values under specified registry item.'''
    key_to_enum = _OpenKeyToEnum(key, sub_key)
    sub_key_cnt, val_cnt, last_modified = winreg.QueryInfoKey(key_to_enum)

    for i in range(val_cnt):
        yield ('', ) + winreg.EnumValue(key_to_enum, i)
        
    if recursive:
        for new_sub_key in EnumSubKey(key, sub_key, True):
            for parent_key, name, data, data_type in \
                EnumValue(key, _Join(sub_key, new_sub_key), True):
                yield (_Join(parent_key, new_sub_key), name, data, data_type)
    
    _CloseKeyToEnum(key, key_to_enum)

def SearchValue(key, sub_key, value_name, recursive = False):
    return filter(lambda value_tuple: value_tuple[1] == value_name,
                  EnumValue(key, sub_key, recursive))
