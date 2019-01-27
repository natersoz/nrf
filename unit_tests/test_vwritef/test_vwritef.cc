/**
 * @file test_int_to_string.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "vwritef.h"
#include "std_stream.h"

#include <iostream>
#include <iomanip>
#include <cstdio>

int main(int argc, char **argv)
{
    int result = 0;

    io::stdout_stream os;

    writef(os, "123:   %10u\n", 123);
    writef(os, "0x123: 0x%08x\n", 0x123);

    writef(os, "'c': '%c'\n", 'c');
    writef(os, "'%%': '%%'\n");

    writef(os, "+123:   %+d\n", 123);
    writef(os, " 123:   % d\n", 123);
    writef(os, "-123:   %+d\n", -123);

    writef(os, "%d, %d, %d, %d, %d, %d, %d\n", 1, 2, 3, 4, 5, 6, 7);
    writef(os, "%x, %x, %x, %x, %x, %x, %x\n",
            0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70);

    writef(os, "%s, %s, %s, %s, %s, %s, %s\n",
            "the", "lazy", "brown", "fox", "jumped", "over", "there");

    writef(os, "'%10s'\n'%10s'\n'%10s'\n'%10s'\n'%10s'\n'%10s'\n'%10s'\n",
            "the", "lazy", "brown", "fox", "jumped", "over", "there");

    writef(os, "'%-10s'\n'%-10s'\n'%-10s'\n'%-10s'\n'%-10s'\n'%-10s'\n'%-10s'\n",
            "the", "lazy", "brown", "fox", "jumped", "over", "there");


    writef(os, "%lld, %lld, %lld, %lld\n",
            0x123456789abcdefull, 0x23456789abcdefull, 0x3456789abcdefull, 0x456789abcdefull);

    writef(os, "%llu, %llu, %llx, %llx\n",
            0x123456789abcdefull, 0x23456789abcdefull, 0x3456789abcdefull, 0x456789abcdefull);

    return result;
}
