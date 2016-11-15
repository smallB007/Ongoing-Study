﻿// 2016-06-14T17:54+08:00
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;


namespace Common.IO
{
    public static class PathUtils
    {
        public static bool IsFile(string path)
        {
            try
            {
                var attr = File.GetAttributes(path);
                return !attr.HasFlag(FileAttributes.Directory);
            }
            catch (Exception e)
            {
                Debug.WriteLine(e);
                return false;
            }
        }

        public static bool IsDirectory(string path)
        {
            try
            {
                var attr = File.GetAttributes(path);
                return attr.HasFlag(FileAttributes.Directory);
            }
            catch (Exception e)
            {
                Debug.WriteLine(e);
                return false;
            }
        }

        public static bool IsAbsolute(string path)
        {
            return Path.IsPathRooted(path);
        }

        public static string GetRelativePath(string fullPath, string startPath)
        {
            var fullPathUri = new Uri(fullPath);
            var startPathUri = new Uri(startPath);

            return startPathUri.MakeRelativeUri(fullPathUri).ToString();
        }
    }

    public static class DirUtils
    {
        public static string[] ListDir(string path, bool silence = true)
        {
            try
            {
                return Directory.GetFileSystemEntries(path);
            }
            catch (Exception e) // DirectoryNotFoundException,SecurityException,ArgumentNullException
            {
                Debug.WriteLine(e);
                if (!silence)
                    throw;

                return new string[0];
            }
        }

        public static IEnumerable<string> Walk(string path, bool recursively = true, bool topdown = true, bool silence = true)
        {
            if (!PathUtils.IsAbsolute(path))
                path = Path.GetFullPath(path);

            IEnumerable<string> entries = recursively ? WalkRecursively(path, topdown, silence) : ListDir(path, silence);
            foreach (var entry in entries)
                yield return entry;
        }

        private delegate string FullPathDelegate(string entry);

        private static IEnumerable<string> WalkRecursively(string path, bool topdown, bool silence)
        {
            FullPathDelegate FullPath = entry => Path.GetFullPath(Path.Combine(path, entry));

            foreach (var entry in ListDir(path, silence))
            {
                var fullPath = FullPath(entry);

                if (topdown)
                    yield return fullPath;

                if (PathUtils.IsDirectory(fullPath))
                {
                    Debug.WriteLine("Directory: " + fullPath);
                    foreach (var subEntry in WalkRecursively(fullPath, topdown, silence))
                        yield return subEntry;
                }

                if (!topdown)
                    yield return fullPath;
            }
        }
    }
}

// References:
// [IEnumerable and Recursion using yield return](http://stackoverflow.com/questions/2055927/ienumerable-and-recursion-using-yield-return)
// [How to: Iterate Through a Directory Tree (C# Programming Guide)](https://msdn.microsoft.com/en-us/library/bb513869.aspx)
// [Walk a directory/Recursively](http://rosettacode.org/wiki/Walk_a_directory/Recursively#C.23)
// [Better way to check if Path is a File or a Directory?](http://stackoverflow.com/questions/1395205/better-way-to-check-if-path-is-a-file-or-a-directory)
// [Check if full path given](http://stackoverflow.com/questions/5565029/check-if-full-path-given)
// [How to Convert Absolute File Path To Relative Path in C#?](https://social.msdn.microsoft.com/Forums/zh-CN/954346c8-cbe8-448c-80d0-d3fc27796e9c/how-to-convert-absolute-file-path-to-relative-path-in-c?forum=csharpgeneral)
