// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

char *strcpy(char *destination, const char *source)
{
	/* TODO: Implement strcpy(). - DONE */
	size_t i = 0;

	while (source[i] != '\0') {
		destination[i] = source [i];
		i++;
	}

	destination[i] = '\0';

	return destination;
}

char *strncpy(char *destination, const char *source, size_t len)
{
	/* TODO: Implement strncpy(). - DONE */
	size_t i;

	for (i = 0; i < len; i++) {
		if (source[i] != '\0')
			destination[i] = source [i];
		else
			break;
	}

	for ( ; i < len; i++) {
		destination[i] = '\0';
	}

	return destination;
}

char *strcat(char *destination, const char *source)
{
	/* TODO: Implement strcat(). - DONE */
	size_t i = 0;
	size_t d_len = strlen(destination);

	while (source[i] != '\0') {
		destination[d_len + i] = source [i];
		i++;
	}

	destination[d_len + i] = '\0';

	return destination;
}

char *strncat(char *destination, const char *source, size_t len)
{
	/* TODO: Implement strncat(). - DONE */
	size_t i;
	size_t d_len = strlen(destination);

	for (i = 0; i < len; i++) {
		if (source[i] != '\0')
			destination[d_len + i] = source [i];
		else
			break;
	}

	destination[d_len + i] = '\0';

	return destination;
}

int strcmp(const char *str1, const char *str2)
{
	/* TODO: Implement strcmp(). - DONE */
	size_t i, ok = 0;
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	if (len1 != len2) {
		if (len1 < len2)
			return -1;
		else
			return 1;
	}

	for (i = 0; i < len1; i++) {
		if (ok == 0) {
			if (str1[i] != str2[i]) {
				if(str1[i] < str2[i])
					return -1;
				else
					return 1;
			}
		}
	}

	return 0;
}

int strncmp(const char *str1, const char *str2, size_t len)
{
	/* TODO: Implement strncmp(). - DONE */
	size_t i, ok = 0;

	for (i = 0; i < len; i++) {
		if (ok == 0) {
			if (str1[i] != str2[i]) {
				if(str1[i] < str2[i])
					return -1;
				else
					return 1;
			}
		}
	}

	return 0;
}

size_t strlen(const char *str)
{
	size_t i = 0;

	for (; *str != '\0'; str++, i++)
	;

	return i;
}

char *strchr(const char *str, int c)
{
	/* TODO: Implement strchr(). - DONE */
	size_t i;
	const char *aux = NULL;

	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == (char)c) {
			aux = &str[i];
			break;
		}
	}

	return (char *)aux;
}

char *strrchr(const char *str, int c)
{
	/* TODO: Implement strrchr(). - DONE */
	size_t i;
	const char *aux = NULL;

	for (i = 0; str[i] != '\0'; i++) {
		if ( str[i] == (char)c) aux = &str[i];
	}

	return (char *)aux;
}

char *strstr(const char *haystack, const char *needle)
{
	/* TODO: Implement strstr(). - DONE */
	int i, j, nr;
    int hay_len = strlen(haystack);
    int needle_len = strlen(needle);

    if (hay_len < needle_len)
		return NULL;

    for (i = 0; i <= hay_len - needle_len; i++) {
		nr = 0;
        for (j = i; needle[nr] != '\0'; j++) {
			if (haystack[j] == needle[nr])
				nr++;
			else
				break;
		}
        if (nr == needle_len)
			return (char *) &haystack[i];
    }

    return NULL;
}

char *strrstr(const char *haystack, const char *needle)
{
	/* TODO: Implement strrstr(). - DONE */
	int i, j, nr;
    int hay_len = strlen(haystack);
    int needle_len = strlen(needle);

    if (hay_len < needle_len)
		return NULL;

    for (i = hay_len - needle_len; i >= 0; i--) {
		nr = 0;
        for (j = i; needle[nr] != '\0'; j++) {
			if (haystack[j] == needle[nr])
				nr++;
			else
				break;
		}
        if (nr == needle_len)
			return (char *) &haystack[i];
    }

    return NULL;
}

void *memcpy(void *destination, const void *source, size_t num)
{
	/* TODO: Implement memcpy(). - DONE */
	size_t i;
	char* dest = (char*)destination;
	char* src = (char*)source;

	for (i = 0; i < num; i++) {
		dest[i] = src[i];
	}

	return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
	/* TODO: Implement memmove(). - DONE */
	char* dest = (char*)destination;
	char* src = (char*)source;

	if (dest < src) {
		while (num > 0) {
			*dest++ = *src++;
			num--;
		}
	} else {
		char *d = dest + (num-1);
		char *s = src + (num-1);
		while (num > 0) {
			*d-- = *s--;
			num--;
		}
	}

	return destination;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	/* TODO: Implement memcmp(). - DONE */
	size_t i;
	char* str1 = (char*)ptr1;
	char* str2 = (char*)ptr2;

	for (i = 0; i < num; i++) {
		if(*str1++ != *str2++)
			return (*--str1 - *--str2);
	}

	return (0);
}

void *memset(void *source, int value, size_t num)
{
	/* TODO: Implement memset(). - DONE */
	size_t i;
	char* src = (char*)source;

	for (i = 0; i < num; i++) {
		src[i] = (char)value;
	}

	return source;
}
