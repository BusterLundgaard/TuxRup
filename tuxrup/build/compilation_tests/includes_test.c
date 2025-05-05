#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum CXChildVisitResult visitIncludes(CXCursor cursor, CXCursor parent, CXClientData client_data);

// Extract the include string from the source range
char *get_include_spelling(CXTranslationUnit tu, CXSourceRange range) {
    CXToken *tokens = NULL;
    unsigned numTokens = 0;
    clang_tokenize(tu, range, &tokens, &numTokens);

    for (unsigned i = 0; i < numTokens; ++i) {
        CXString spelling = clang_getTokenSpelling(tu, tokens[i]);
        const char *token = clang_getCString(spelling);
        if (token && (token[0] == '<' || token[0] == '"')) {
            char *result = strdup(token);
            clang_disposeString(spelling);
            clang_disposeTokens(tu, tokens, numTokens);
            return result;
        }
        clang_disposeString(spelling);
    }

    clang_disposeTokens(tu, tokens, numTokens);
    return NULL;
}

enum CXChildVisitResult visitIncludes(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	/* printf("cursor kind is %s\n", clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(cursor)))); */
    if (clang_getCursorKind(cursor) == CXCursor_InclusionDirective) {
        CXTranslationUnit tu = (CXTranslationUnit)client_data;
        CXSourceRange range = clang_getCursorExtent(cursor);

        char *include_text = get_include_spelling(tu, range);
		printf("includes_text is %s\n", include_text);
        if (include_text) {
            if (include_text[0] == '<') {
                printf("System include: %s\n", include_text);
            } else if (include_text[0] == '"') {
                printf("User include: %s\n", include_text);
            } else {
                printf("Unknown include: %s\n", include_text);
            }
            free(include_text);
        }
    }

	return CXChildVisit_Continue;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
	printf("filename is %s\n", filename);
    CXIndex index = clang_createIndex(0, 0);

    CXTranslationUnit tu = clang_parseTranslationUnit(
        index, filename, NULL, 0, NULL, 0, CXTranslationUnit_DetailedPreprocessingRecord);

    if (!tu) {
        fprintf(stderr, "Failed to parse translation unit\n");
        return 1;
    }

    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(cursor, visitIncludes, tu);

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);
    return 0;
}

