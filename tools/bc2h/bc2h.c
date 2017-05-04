
#include <stdio.h>

int
main(int argc, char **argv)
{
    FILE *ifp;
    FILE *ofp;
    int c, i, l, s;

    if (argc != 4) {
        fprintf(stderr, "usage: bc2h <input .bc path> <output .h path> <array name>\n");
        return 1;
    }

    ifp = fopen(argv[1], "rb");
    if (!ifp) {
        fprintf(stderr, "Could not open \"%s\" for reading\n", argv[1]);
        return 1;
    }

    s = fseek(ifp, 0, SEEK_END);
    if (s < 0) {
        fprintf(stderr, "Could not seek \"%s\"\n", argv[1]);
        return 1;
    }

    l = ftell(ifp);
    if (l < 0) {
        fprintf(stderr, "Could not tell \"%s\"\n", argv[1]);
        return 1;
    }

    s = fseek(ifp, 0, SEEK_SET);
    if (s < 0) {
        fprintf(stderr, "Could not seek \"%s\"\n", argv[1]);
        return 1;
    }

    ofp = fopen(argv[2], "wb+");
    if (!ofp) {
        fprintf(stderr, "Could not open \"%s\" for writing\n", argv[2]);
        return 1;
    }

    fprintf(ofp, "// This file generated automatically by bc2h\n"
                 "// DO NOT EDIT\n\n"
                 "#define %s_size %d\n\n"
                 "#if defined __GNUC__\n"
                 "__attribute__((aligned (4096)))\n"
                 "#elif defined _MSC_VER\n"
                 "__declspec(align(4096))\n"
                 "#endif\n"
                 "static const unsigned char %s[%s_size+1] = {\n",
                 argv[3], l,
                 argv[3], argv[3]);

    fprintf(ofp, "    ");
    i = 0;
    while ((c = getc(ifp)) != EOF) {
        ++i;
        if (i < 8)
            fprintf(ofp, "0x%02x, ", c);
        else {
            fprintf(ofp, "0x%02x,\n    ", c);
            i = 0;
        }
    }

    fprintf(ofp, "0x00\n};\n\n");
    fclose(ifp);
    fclose(ofp);
    return 0;
}

