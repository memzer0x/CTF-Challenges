/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <jni.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

JNIEnv* env;

typedef uint32_t u4;
typedef uint8_t u1;

#define CHECK_METHOD_OFFSET 0x720
#define PATCH_SIZE 0x90
#define DEX_SIZE 5544

const u1 dex[] = {
  100, 101, 120, 10, 48, 51, 53, 0, 169, 234, 218, 146, 39, 34, 214, 191, 68, 193, 26, 214, 71, 213, 129, 123, 179, 123, 152, 33, 195, 0, 203, 47, 168, 21, 0, 0, 112, 0, 0, 0, 120, 86, 52, 18, 0, 0, 0, 0, 0, 0, 0, 0, 216, 20, 0, 0, 123, 0, 0, 0, 112, 0, 0, 0, 35, 0, 0, 0, 92, 2, 0, 0, 24, 0, 0, 0, 232, 2, 0, 0, 12, 0, 0, 0, 8, 4, 0, 0, 38, 0, 0, 0, 104, 4, 0, 0, 6, 0, 0, 0, 152, 5, 0, 0, 80, 15, 0, 0, 88, 6, 0, 0, 16, 13, 0, 0, 26, 13, 0, 0, 36, 13, 0, 0, 44, 13, 0, 0, 47, 13, 0, 0, 55, 13, 0, 0, 61, 13, 0, 0, 75, 13, 0, 0, 78, 13, 0, 0, 82, 13, 0, 0, 85, 13, 0, 0, 89, 13, 0, 0, 94, 13, 0, 0, 99, 13, 0, 0, 105, 13, 0, 0, 129, 13, 0, 0, 166, 13, 0, 0, 193, 13, 0, 0, 219, 13, 0, 0, 251, 13, 0, 0, 28, 14, 0, 0, 49, 14, 0, 0, 80, 14, 0, 0, 107, 14, 0, 0, 144, 14, 0, 0, 165, 14, 0, 0, 204, 14, 0, 0, 230, 14, 0, 0, 255, 14, 0, 0, 41, 15, 0, 0, 70, 15, 0, 0, 94, 15, 0, 0, 119, 15, 0, 0, 147, 15, 0, 0, 172, 15, 0, 0, 199, 15, 0, 0, 224, 15, 0, 0, 251, 15, 0, 0, 31, 16, 0, 0, 68, 16, 0, 0, 100, 16, 0, 0, 135, 16, 0, 0, 161, 16, 0, 0, 181, 16, 0, 0, 201, 16, 0, 0, 209, 16, 0, 0, 217, 16, 0, 0, 220, 16, 0, 0, 224, 16, 0, 0, 228, 16, 0, 0, 234, 16, 0, 0, 239, 16, 0, 0, 243, 16, 0, 0, 8, 17, 0, 0, 11, 17, 0, 0, 24, 17, 0, 0, 35, 17, 0, 0, 44, 17, 0, 0, 60, 17, 0, 0, 63, 17, 0, 0, 67, 17, 0, 0, 78, 17, 0, 0, 92, 17, 0, 0, 98, 17, 0, 0, 110, 17, 0, 0, 133, 17, 0, 0, 152, 17, 0, 0, 163, 17, 0, 0, 171, 17, 0, 0, 185, 17, 0, 0, 193, 17, 0, 0, 197, 17, 0, 0, 200, 17, 0, 0, 210, 17, 0, 0, 216, 17, 0, 0, 225, 17, 0, 0, 236, 17, 0, 0, 253, 17, 0, 0, 7, 18, 0, 0, 25, 18, 0, 0, 40, 18, 0, 0, 57, 18, 0, 0, 77, 18, 0, 0, 86, 18, 0, 0, 92, 18, 0, 0, 100, 18, 0, 0, 107, 18, 0, 0, 115, 18, 0, 0, 122, 18, 0, 0, 129, 18, 0, 0, 134, 18, 0, 0, 144, 18, 0, 0, 152, 18, 0, 0, 160, 18, 0, 0, 168, 18, 0, 0, 176, 18, 0, 0, 184, 18, 0, 0, 192, 18, 0, 0, 200, 18, 0, 0, 208, 18, 0, 0, 216, 18, 0, 0, 224, 18, 0, 0, 232, 18, 0, 0, 240, 18, 0, 0, 248, 18, 0, 0, 0, 19, 0, 0, 8, 19, 0, 0, 16, 19, 0, 0, 24, 19, 0, 0, 32, 19, 0, 0, 40, 19, 0, 0, 48, 19, 0, 0, 56, 19, 0, 0, 64, 19, 0, 0, 72, 19, 0, 0, 80, 19, 0, 0, 88, 19, 0, 0, 96, 19, 0, 0, 104, 19, 0, 0, 112, 19, 0, 0, 120, 19, 0, 0, 128, 19, 0, 0, 136, 19, 0, 0, 3, 0, 0, 0, 7, 0, 0, 0, 14, 0, 0, 0, 15, 0, 0, 0, 16, 0, 0, 0, 17, 0, 0, 0, 18, 0, 0, 0, 19, 0, 0, 0, 20, 0, 0, 0, 21, 0, 0, 0, 22, 0, 0, 0, 23, 0, 0, 0, 24, 0, 0, 0, 25, 0, 0, 0, 26, 0, 0, 0, 27, 0, 0, 0, 28, 0, 0, 0, 29, 0, 0, 0, 30, 0, 0, 0, 31, 0, 0, 0, 32, 0, 0, 0, 33, 0, 0, 0, 34, 0, 0, 0, 35, 0, 0, 0, 36, 0, 0, 0, 37, 0, 0, 0, 38, 0, 0, 0, 39, 0, 0, 0, 40, 0, 0, 0, 41, 0, 0, 0, 42, 0, 0, 0, 43, 0, 0, 0, 46, 0, 0, 0, 51, 0, 0, 0, 52, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 132, 12, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 144, 12, 0, 0, 9, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 5, 0, 0, 0, 152, 12, 0, 0, 11, 0, 0, 0, 5, 0, 0, 0, 160, 12, 0, 0, 9, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 12, 0, 0, 0, 168, 12, 0, 0, 13, 0, 0, 0, 18, 0, 0, 0, 176, 12, 0, 0, 46, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 32, 0, 0, 0, 168, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 188, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 196, 12, 0, 0, 50, 0, 0, 0, 32, 0, 0, 0, 204, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 212, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 124, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 220, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 228, 12, 0, 0, 49, 0, 0, 0, 32, 0, 0, 0, 236, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 248, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 144, 12, 0, 0, 48, 0, 0, 0, 32, 0, 0, 0, 0, 13, 0, 0, 12, 0, 0, 0, 33, 0, 0, 0, 8, 13, 0, 0, 16, 0, 1, 0, 69, 0, 0, 0, 16, 0, 1, 0, 88, 0, 0, 0, 19, 0, 2, 0, 53, 0, 0, 0, 19, 0, 1, 0, 58, 0, 0, 0, 19, 0, 33, 0, 62, 0, 0, 0, 19, 0, 33, 0, 71, 0, 0, 0, 20, 0, 1, 0, 63, 0, 0, 0, 22, 0, 23, 0, 84, 0, 0, 0, 22, 0, 2, 0, 85, 0, 0, 0, 22, 0, 1, 0, 86, 0, 0, 0, 23, 0, 31, 0, 7, 0, 0, 0, 23, 0, 2, 0, 53, 0, 0, 0, 2, 0, 8, 0, 61, 0, 0, 0, 2, 0, 2, 0, 64, 0, 0, 0, 2, 0, 5, 0, 68, 0, 0, 0, 2, 0, 3, 0, 78, 0, 0, 0, 2, 0, 15, 0, 79, 0, 0, 0, 3, 0, 10, 0, 2, 0, 0, 0, 5, 0, 21, 0, 2, 0, 0, 0, 5, 0, 6, 0, 66, 0, 0, 0, 5, 0, 4, 0, 77, 0, 0, 0, 6, 0, 10, 0, 2, 0, 0, 0, 6, 0, 21, 0, 55, 0, 0, 0, 7, 0, 7, 0, 65, 0, 0, 0, 8, 0, 1, 0, 67, 0, 0, 0, 10, 0, 0, 0, 57, 0, 0, 0, 12, 0, 11, 0, 76, 0, 0, 0, 15, 0, 13, 0, 2, 0, 0, 0, 15, 0, 18, 0, 80, 0, 0, 0, 15, 0, 16, 0, 81, 0, 0, 0, 15, 0, 20, 0, 82, 0, 0, 0, 16, 0, 10, 0, 2, 0, 0, 0, 17, 0, 17, 0, 56, 0, 0, 0, 18, 0, 9, 0, 72, 0, 0, 0, 18, 0, 10, 0, 83, 0, 0, 0, 19, 0, 10, 0, 1, 0, 0, 0, 19, 0, 12, 0, 2, 0, 0, 0, 19, 0, 10, 0, 59, 0, 0, 0, 19, 0, 14, 0, 75, 0, 0, 0, 20, 0, 10, 0, 2, 0, 0, 0, 21, 0, 10, 0, 2, 0, 0, 0, 22, 0, 19, 0, 2, 0, 0, 0, 22, 0, 17, 0, 74, 0, 0, 0, 23, 0, 10, 0, 1, 0, 0, 0, 23, 0, 12, 0, 2, 0, 0, 0, 24, 0, 10, 0, 2, 0, 0, 0, 24, 0, 23, 0, 89, 0, 0, 0, 30, 0, 10, 0, 2, 0, 0, 0, 31, 0, 22, 0, 2, 0, 0, 0, 31, 0, 1, 0, 60, 0, 0, 0, 19, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 103, 20, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 17, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 44, 0, 0, 0, 76, 12, 0, 0, 135, 20, 0, 0, 97, 20, 0, 0, 21, 0, 0, 0, 17, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 44, 0, 0, 0, 92, 12, 0, 0, 147, 20, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 124, 12, 0, 0, 45, 0, 0, 0, 108, 12, 0, 0, 157, 20, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 1, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 180, 20, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 1, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 90, 0, 0, 0, 0, 0, 0, 0, 200, 20, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 60, 20, 0, 0, 66, 20, 0, 0, 1, 0, 0, 0, 75, 20, 0, 0, 2, 0, 0, 0, 83, 20, 0, 0, 89, 20, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 144, 19, 0, 0, 33, 0, 0, 0, 19, 0, 38, 0, 35, 0, 33, 0, 38, 0, 6, 0, 0, 0, 105, 0, 4, 0, 14, 0, 0, 3, 1, 0, 38, 0, 0, 0, 237, 116, 58, 108, 255, 33, 9, 61, 195, 219, 108, 133, 3, 35, 97, 246, 241, 15, 171, 190, 225, 191, 17, 79, 31, 25, 217, 95, 93, 1, 146, 153, 138, 218, 199, 198, 205, 177, 0, 0, 6, 0, 2, 0, 1, 0, 0, 0, 149, 19, 0, 0, 25, 0, 0, 0, 19, 3, 8, 0, 18, 1, 112, 16, 5, 0, 4, 0, 91, 69, 2, 0, 35, 48, 33, 0, 91, 64, 5, 0, 1, 16, 53, 48, 9, 0, 84, 66, 5, 0, 79, 1, 2, 0, 216, 0, 0, 1, 40, 248, 89, 65, 3, 0, 14, 0, 0, 0, 6, 0, 1, 0, 3, 0, 0, 0, 162, 19, 0, 0, 72, 0, 0, 0, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 8, 0, 3, 0, 2, 0, 0, 0, 178, 19, 0, 0, 50, 0, 0, 0, 19, 4, 8, 0, 18, 1, 110, 16, 7, 0, 7, 0, 12, 0, 26, 2, 70, 0, 110, 32, 12, 0, 32, 0, 10, 0, 84, 82, 5, 0, 82, 83, 3, 0, 141, 0, 79, 0, 2, 3, 110, 16, 25, 0, 5, 0, 82, 80, 3, 0, 216, 0, 0, 1, 89, 80, 3, 0, 82, 80, 3, 0, 51, 64, 18, 0, 89, 81, 3, 0, 35, 64, 33, 0, 91, 80, 5, 0, 1, 16, 53, 64, 9, 0, 84, 82, 5, 0, 79, 1, 2, 0, 216, 0, 0, 1, 40, 248, 14, 0, 1, 0, 1, 0, 1, 0, 0, 0, 195, 19, 0, 0, 4, 0, 0, 0, 112, 16, 35, 0, 0, 0, 14, 0, 1, 0, 1, 0, 1, 0, 0, 0, 200, 19, 0, 0, 4, 0, 0, 0, 112, 16, 35, 0, 0, 0, 14, 0, 4, 0, 4, 0, 1, 0, 0, 0, 206, 19, 0, 0, 10, 0, 0, 0, 91, 1, 7, 0, 89, 2, 9, 0, 91, 3, 8, 0, 112, 16, 35, 0, 0, 0, 14, 0, 5, 0, 2, 0, 3, 0, 0, 0, 214, 19, 0, 0, 24, 0, 0, 0, 18, 0, 110, 32, 14, 0, 4, 0, 34, 0, 5, 0, 98, 1, 10, 0, 112, 32, 6, 0, 16, 0, 26, 1, 70, 0, 82, 50, 9, 0, 110, 48, 8, 0, 16, 2, 84, 49, 8, 0, 110, 32, 4, 0, 1, 0, 14, 0, 1, 0, 0, 0, 0, 0, 0, 0, 224, 19, 0, 0, 5, 0, 0, 0, 26, 0, 6, 0, 105, 0, 10, 0, 14, 0, 0, 0, 12, 0, 2, 0, 4, 0, 0, 0, 229, 19, 0, 0, 35, 1, 0, 0, 19, 9, 32, 0, 18, 1, 21, 8, 112, 66, 18, 23, 112, 16, 35, 0, 10, 0, 105, 11, 11, 0, 110, 16, 1, 0, 11, 0, 12, 2, 20, 0, 80, 0, 12, 127, 110, 32, 0, 0, 11, 0, 12, 0, 31, 0, 17, 0, 35, 147, 34, 0, 26, 4, 107, 0, 77, 4, 3, 1, 26, 4, 122, 0, 77, 4, 3, 7, 18, 36, 26, 5, 114, 0, 77, 5, 3, 4, 18, 52, 26, 5, 103, 0, 77, 5, 3, 4, 18, 68, 26, 5, 95, 0, 77, 5, 3, 4, 18, 84, 26, 5, 110, 0, 77, 5, 3, 4, 18, 100, 26, 5, 113, 0, 77, 5, 3, 4, 18, 116, 26, 5, 105, 0, 77, 5, 3, 4, 19, 4, 8, 0, 26, 5, 98, 0, 77, 5, 3, 4, 19, 4, 9, 0, 26, 5, 99, 0, 77, 5, 3, 4, 19, 4, 10, 0, 26, 5, 91, 0, 77, 5, 3, 4, 19, 4, 11, 0, 26, 5, 109, 0, 77, 5, 3, 4, 19, 4, 12, 0, 26, 5, 118, 0, 77, 5, 3, 4, 19, 4, 13, 0, 26, 5, 119, 0, 77, 5, 3, 4, 19, 4, 14, 0, 26, 5, 97, 0, 77, 5, 3, 4, 19, 4, 15, 0, 26, 5, 101, 0, 77, 5, 3, 4, 19, 4, 16, 0, 26, 5, 117, 0, 77, 5, 3, 4, 19, 4, 17, 0, 26, 5, 94, 0, 77, 5, 3, 4, 19, 4, 18, 0, 26, 5, 100, 0, 77, 5, 3, 4, 19, 4, 19, 0, 26, 5, 104, 0, 77, 5, 3, 4, 19, 4, 20, 0, 26, 5, 116, 0, 77, 5, 3, 4, 19, 4, 21, 0, 26, 5, 102, 0, 77, 5, 3, 4, 19, 4, 22, 0, 26, 5, 96, 0, 77, 5, 3, 4, 19, 4, 23, 0, 26, 5, 115, 0, 77, 5, 3, 4, 19, 4, 24, 0, 26, 5, 106, 0, 77, 5, 3, 4, 19, 4, 25, 0, 26, 5, 112, 0, 77, 5, 3, 4, 19, 4, 26, 0, 26, 5, 121, 0, 77, 5, 3, 4, 19, 4, 27, 0, 26, 5, 111, 0, 77, 5, 3, 4, 19, 4, 28, 0, 26, 5, 120, 0, 77, 5, 3, 4, 19, 4, 29, 0, 26, 5, 93, 0, 77, 5, 3, 4, 19, 4, 30, 0, 26, 5, 92, 0, 77, 5, 3, 4, 19, 4, 31, 0, 26, 5, 108, 0, 77, 5, 3, 4, 53, 145, 64, 0, 34, 4, 15, 0, 112, 32, 15, 0, 36, 0, 34, 5, 16, 0, 112, 16, 19, 0, 5, 0, 110, 16, 2, 0, 11, 0, 12, 6, 110, 16, 11, 0, 6, 0, 12, 6, 113, 48, 13, 0, 135, 6, 10, 6, 135, 102, 89, 86, 1, 0, 110, 16, 2, 0, 11, 0, 12, 6, 110, 16, 11, 0, 6, 0, 12, 6, 113, 48, 13, 0, 135, 6, 10, 6, 135, 102, 89, 86, 0, 0, 110, 32, 16, 0, 84, 0, 70, 5, 3, 1, 110, 32, 18, 0, 84, 0, 34, 5, 22, 0, 112, 64, 29, 0, 165, 177, 110, 32, 17, 0, 84, 0, 110, 32, 20, 0, 64, 0, 216, 1, 1, 1, 40, 193, 34, 0, 6, 0, 112, 16, 9, 0, 0, 0, 98, 1, 10, 0, 110, 32, 10, 0, 16, 0, 34, 1, 19, 0, 112, 32, 24, 0, 177, 0, 110, 48, 3, 0, 27, 0, 14, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 8, 20, 0, 0, 4, 0, 0, 0, 112, 16, 35, 0, 0, 0, 14, 0, 10, 0, 2, 0, 0, 0, 0, 0, 13, 20, 0, 0, 123, 0, 0, 0, 19, 7, 0, 1, 35, 115, 33, 0, 35, 116, 33, 0, 18, 0, 1, 1, 51, 113, 16, 0, 183, 17, 1, 18, 1, 1, 51, 114, 24, 0, 33, 132, 35, 68, 33, 0, 183, 34, 183, 17, 33, 133, 51, 80, 51, 0, 17, 4, 141, 18, 79, 2, 3, 1, 33, 146, 148, 2, 1, 2, 72, 2, 9, 2, 79, 2, 4, 1, 216, 1, 1, 1, 40, 228, 72, 5, 3, 2, 176, 81, 72, 5, 4, 2, 176, 81, 213, 17, 255, 0, 72, 5, 3, 1, 72, 6, 3, 2, 183, 101, 141, 85, 79, 5, 3, 1, 72, 5, 3, 2, 72, 6, 3, 1, 183, 101, 141, 85, 79, 5, 3, 2, 72, 5, 3, 1, 72, 6, 3, 2, 183, 101, 141, 85, 79, 5, 3, 1, 216, 2, 2, 1, 40, 198, 216, 2, 2, 1, 213, 34, 255, 0, 72, 5, 3, 2, 176, 81, 213, 17, 255, 0, 72, 5, 3, 1, 72, 6, 3, 2, 183, 101, 141, 85, 79, 5, 3, 1, 72, 5, 3, 2, 72, 6, 3, 1, 183, 101, 141, 85, 79, 5, 3, 2, 72, 5, 3, 1, 72, 6, 3, 2, 183, 101, 141, 85, 79, 5, 3, 1, 72, 5, 8, 0, 72, 6, 3, 2, 72, 7, 3, 1, 176, 118, 213, 102, 255, 0, 72, 6, 3, 6, 183, 101, 141, 85, 79, 5, 4, 0, 216, 0, 0, 1, 40, 154, 0, 0, 88, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 11, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 9, 0, 0, 0, 1, 0, 0, 0, 31, 0, 0, 0, 2, 0, 0, 0, 3, 0, 6, 0, 2, 0, 0, 0, 31, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 4, 0, 29, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 4, 0, 5, 0, 1, 0, 0, 0, 5, 0, 0, 0, 1, 0, 0, 0, 12, 0, 0, 0, 1, 0, 0, 0, 13, 0, 0, 0, 3, 0, 0, 0, 23, 0, 1, 0, 2, 0, 0, 0, 1, 0, 0, 0, 29, 0, 0, 0, 1, 0, 0, 0, 33, 0, 0, 0, 2, 0, 0, 0, 33, 0, 33, 0, 8, 19, 17, 19, 3, 4, 3, 1, 5, 0, 8, 60, 99, 108, 105, 110, 105, 116, 62, 0, 6, 60, 105, 110, 105, 116, 62, 0, 1, 70, 0, 6, 70, 46, 106, 97, 118, 97, 0, 4, 70, 73, 70, 76, 0, 12, 70, 76, 65, 71, 95, 70, 65, 67, 84, 79, 82, 89, 0, 1, 73, 0, 2, 73, 76, 0, 1, 76, 0, 2, 76, 73, 0, 3, 76, 76, 73, 0, 3, 76, 76, 76, 0, 4, 76, 76, 76, 73, 0, 22, 76, 97, 110, 100, 114, 111, 105, 100, 47, 97, 112, 112, 47, 65, 99, 116, 105, 118, 105, 116, 121, 59, 0, 35, 76, 97, 110, 100, 114, 111, 105, 100, 47, 99, 111, 110, 116, 101, 110, 116, 47, 66, 114, 111, 97, 100, 99, 97, 115, 116, 82, 101, 99, 101, 105, 118, 101, 114, 59, 0, 25, 76, 97, 110, 100, 114, 111, 105, 100, 47, 99, 111, 110, 116, 101, 110, 116, 47, 67, 111, 110, 116, 101, 120, 116, 59, 0, 24, 76, 97, 110, 100, 114, 111, 105, 100, 47, 99, 111, 110, 116, 101, 110, 116, 47, 73, 110, 116, 101, 110, 116, 59, 0, 30, 76, 97, 110, 100, 114, 111, 105, 100, 47, 99, 111, 110, 116, 101, 110, 116, 47, 73, 110, 116, 101, 110, 116, 70, 105, 108, 116, 101, 114, 59, 0, 31, 76, 97, 110, 100, 114, 111, 105, 100, 47, 99, 111, 110, 116, 101, 110, 116, 47, 114, 101, 115, 47, 82, 101, 115, 111, 117, 114, 99, 101, 115, 59, 0, 19, 76, 97, 110, 100, 114, 111, 105, 100, 47, 111, 115, 47, 66, 117, 110, 100, 108, 101, 59, 0, 29, 76, 97, 110, 100, 114, 111, 105, 100, 47, 117, 116, 105, 108, 47, 68, 105, 115, 112, 108, 97, 121, 77, 101, 116, 114, 105, 99, 115, 59, 0, 25, 76, 97, 110, 100, 114, 111, 105, 100, 47, 117, 116, 105, 108, 47, 84, 121, 112, 101, 100, 86, 97, 108, 117, 101, 59, 0, 35, 76, 97, 110, 100, 114, 111, 105, 100, 47, 118, 105, 101, 119, 47, 86, 105, 101, 119, 36, 79, 110, 67, 108, 105, 99, 107, 76, 105, 115, 116, 101, 110, 101, 114, 59, 0, 19, 76, 97, 110, 100, 114, 111, 105, 100, 47, 118, 105, 101, 119, 47, 86, 105, 101, 119, 59, 0, 37, 76, 97, 110, 100, 114, 111, 105, 100, 47, 118, 105, 101, 119, 47, 86, 105, 101, 119, 71, 114, 111, 117, 112, 36, 76, 97, 121, 111, 117, 116, 80, 97, 114, 97, 109, 115, 59, 0, 24, 76, 97, 110, 100, 114, 111, 105, 100, 47, 118, 105, 101, 119, 47, 86, 105, 101, 119, 71, 114, 111, 117, 112, 59, 0, 23, 76, 97, 110, 100, 114, 111, 105, 100, 47, 119, 105, 100, 103, 101, 116, 47, 66, 117, 116, 116, 111, 110, 59, 0, 40, 76, 97, 110, 100, 114, 111, 105, 100, 47, 119, 105, 100, 103, 101, 116, 47, 71, 114, 105, 100, 76, 97, 121, 111, 117, 116, 36, 76, 97, 121, 111, 117, 116, 80, 97, 114, 97, 109, 115, 59, 0, 27, 76, 97, 110, 100, 114, 111, 105, 100, 47, 119, 105, 100, 103, 101, 116, 47, 71, 114, 105, 100, 76, 97, 121, 111, 117, 116, 59, 0, 22, 76, 97, 110, 100, 114, 111, 105, 100, 47, 119, 105, 100, 103, 101, 116, 47, 84, 111, 97, 115, 116, 59, 0, 23, 76, 99, 111, 109, 47, 103, 111, 111, 103, 108, 101, 47, 99, 116, 102, 47, 102, 111, 111, 100, 47, 70, 59, 0, 26, 76, 99, 111, 109, 47, 103, 111, 111, 103, 108, 101, 47, 99, 116, 102, 47, 102, 111, 111, 100, 47, 82, 36, 105, 100, 59, 0, 23, 76, 99, 111, 109, 47, 103, 111, 111, 103, 108, 101, 47, 99, 116, 102, 47, 102, 111, 111, 100, 47, 82, 59, 0, 25, 76, 99, 111, 109, 47, 103, 111, 111, 103, 108, 101, 47, 99, 116, 102, 47, 102, 111, 111, 100, 47, 83, 36, 49, 59, 0, 23, 76, 99, 111, 109, 47, 103, 111, 111, 103, 108, 101, 47, 99, 116, 102, 47, 102, 111, 111, 100, 47, 83, 59, 0, 23, 76, 99, 111, 109, 47, 103, 111, 111, 103, 108, 101, 47, 99, 116, 102, 47, 102, 111, 111, 100, 47, 226, 132, 157, 59, 0, 34, 76, 100, 97, 108, 118, 105, 107, 47, 97, 110, 110, 111, 116, 97, 116, 105, 111, 110, 47, 69, 110, 99, 108, 111, 115, 105, 110, 103, 67, 108, 97, 115, 115, 59, 0, 35, 76, 100, 97, 108, 118, 105, 107, 47, 97, 110, 110, 111, 116, 97, 116, 105, 111, 110, 47, 69, 110, 99, 108, 111, 115, 105, 110, 103, 77, 101, 116, 104, 111, 100, 59, 0, 30, 76, 100, 97, 108, 118, 105, 107, 47, 97, 110, 110, 111, 116, 97, 116, 105, 111, 110, 47, 73, 110, 110, 101, 114, 67, 108, 97, 115, 115, 59, 0, 33, 76, 100, 97, 108, 118, 105, 107, 47, 97, 110, 110, 111, 116, 97, 116, 105, 111, 110, 47, 77, 101, 109, 98, 101, 114, 67, 108, 97, 115, 115, 101, 115, 59, 0, 24, 76, 106, 97, 118, 97, 47, 108, 97, 110, 103, 47, 67, 104, 97, 114, 83, 101, 113, 117, 101, 110, 99, 101, 59, 0, 18, 76, 106, 97, 118, 97, 47, 108, 97, 110, 103, 47, 79, 98, 106, 101, 99, 116, 59, 0, 18, 76, 106, 97, 118, 97, 47, 108, 97, 110, 103, 47, 83, 116, 114, 105, 110, 103, 59, 0, 6, 82, 46, 106, 97, 118, 97, 0, 6, 83, 46, 106, 97, 118, 97, 0, 1, 86, 0, 2, 86, 73, 0, 2, 86, 76, 0, 4, 86, 76, 73, 76, 0, 3, 86, 76, 76, 0, 2, 91, 66, 0, 19, 91, 76, 106, 97, 118, 97, 47, 108, 97, 110, 103, 47, 83, 116, 114, 105, 110, 103, 59, 0, 1, 97, 0, 11, 97, 99, 99, 101, 115, 115, 70, 108, 97, 103, 115, 0, 9, 97, 100, 100, 65, 99, 116, 105, 111, 110, 0, 7, 97, 100, 100, 86, 105, 101, 119, 0, 14, 97, 112, 112, 108, 121, 68, 105, 109, 101, 110, 115, 105, 111, 110, 0, 1, 99, 0, 2, 99, 99, 0, 9, 99, 111, 109, 112, 97, 114, 101, 84, 111, 0, 12, 102, 105, 110, 100, 86, 105, 101, 119, 66, 121, 73, 100, 0, 4, 102, 108, 97, 103, 0, 10, 102, 111, 111, 100, 76, 97, 121, 111, 117, 116, 0, 21, 103, 101, 116, 65, 112, 112, 108, 105, 99, 97, 116, 105, 111, 110, 67, 111, 110, 116, 101, 120, 116, 0, 17, 103, 101, 116, 68, 105, 115, 112, 108, 97, 121, 77, 101, 116, 114, 105, 99, 115, 0, 9, 103, 101, 116, 69, 120, 116, 114, 97, 115, 0, 6, 103, 101, 116, 73, 110, 116, 0, 12, 103, 101, 116, 82, 101, 115, 111, 117, 114, 99, 101, 115, 0, 6, 104, 101, 105, 103, 104, 116, 0, 2, 105, 100, 0, 1, 107, 0, 8, 109, 97, 107, 101, 84, 101, 120, 116, 0, 4, 110, 97, 109, 101, 0, 7, 111, 110, 67, 108, 105, 99, 107, 0, 9, 111, 110, 82, 101, 99, 101, 105, 118, 101, 0, 15, 112, 108, 97, 121, 83, 111, 117, 110, 100, 69, 102, 102, 101, 99, 116, 0, 8, 112, 117, 116, 69, 120, 116, 114, 97, 0, 16, 114, 101, 103, 105, 115, 116, 101, 114, 82, 101, 99, 101, 105, 118, 101, 114, 0, 13, 115, 101, 110, 100, 66, 114, 111, 97, 100, 99, 97, 115, 116, 0, 15, 115, 101, 116, 76, 97, 121, 111, 117, 116, 80, 97, 114, 97, 109, 115, 0, 18, 115, 101, 116, 79, 110, 67, 108, 105, 99, 107, 76, 105, 115, 116, 101, 110, 101, 114, 0, 7, 115, 101, 116, 84, 101, 120, 116, 0, 4, 115, 104, 111, 119, 0, 6, 116, 104, 105, 115, 36, 48, 0, 5, 118, 97, 108, 36, 97, 0, 6, 118, 97, 108, 36, 105, 100, 0, 5, 118, 97, 108, 117, 101, 0, 5, 119, 105, 100, 116, 104, 0, 1, 226, 132, 130, 0, 6, 226, 132, 157, 46, 106, 97, 118, 97, 0, 2, 237, 160, 188, 237, 188, 176, 0, 2, 237, 160, 188, 237, 188, 189, 0, 2, 237, 160, 188, 237, 188, 190, 0, 2, 237, 160, 188, 237, 189, 132, 0, 2, 237, 160, 188, 237, 189, 133, 0, 2, 237, 160, 188, 237, 189, 134, 0, 2, 237, 160, 188, 237, 189, 135, 0, 2, 237, 160, 188, 237, 189, 136, 0, 2, 237, 160, 188, 237, 189, 137, 0, 2, 237, 160, 188, 237, 189, 138, 0, 2, 237, 160, 188, 237, 189, 140, 0, 2, 237, 160, 188, 237, 189, 141, 0, 2, 237, 160, 188, 237, 189, 142, 0, 2, 237, 160, 188, 237, 189, 146, 0, 2, 237, 160, 188, 237, 189, 147, 0, 2, 237, 160, 188, 237, 189, 148, 0, 2, 237, 160, 188, 237, 189, 149, 0, 2, 237, 160, 188, 237, 189, 150, 0, 2, 237, 160, 188, 237, 189, 151, 0, 2, 237, 160, 188, 237, 189, 153, 0, 2, 237, 160, 188, 237, 189, 154, 0, 2, 237, 160, 188, 237, 189, 156, 0, 2, 237, 160, 188, 237, 189, 157, 0, 2, 237, 160, 188, 237, 189, 158, 0, 2, 237, 160, 188, 237, 189, 159, 0, 2, 237, 160, 188, 237, 189, 160, 0, 2, 237, 160, 188, 237, 189, 163, 0, 2, 237, 160, 188, 237, 189, 164, 0, 2, 237, 160, 188, 237, 189, 166, 0, 2, 237, 160, 188, 237, 189, 168, 0, 2, 237, 160, 188, 237, 189, 169, 0, 2, 237, 160, 188, 237, 189, 172, 0, 15, 0, 7, 14, 0, 20, 1, 0, 7, 59, 60, 46, 90, 45, 73, 62, 45, 0, 49, 0, 7, 44, 91, 60, 163, 63, 210, 1, 27, 17, 2, 118, 29, 0, 32, 2, 0, 0, 7, 59, 166, 121, 61, 105, 75, 45, 90, 45, 73, 63, 0, 4, 0, 7, 14, 0, 3, 0, 7, 14, 60, 0, 48, 3, 0, 0, 0, 7, 14, 0, 51, 1, 0, 7, 14, 76, 120, 120, 90, 0, 15, 0, 7, 14, 0, 18, 1, 0, 7, 104, 60, 46, 75, 151, 2, 11, 1, 184, 1, 14, 45, 91, 90, 241, 244, 60, 90, 2, 11, 134, 2, 104, 59, 2, 27, 59, 90, 90, 135, 0, 3, 0, 7, 14, 0, 7, 2, 0, 0, 7, 44, 45, 45, 49, 2, 15, 44, 63, 2, 16, 44, 60, 30, 34, 2, 20, 59, 2, 70, 29, 60, 121, 46, 2, 13, 29, 136, 135, 135, 136, 45, 2, 17, 29, 75, 91, 135, 135, 136, 241, 45, 0, 2, 25, 1, 87, 24, 21, 2, 27, 2, 54, 4, 25, 73, 23, 70, 2, 28, 1, 87, 28, 1, 24, 20, 2, 26, 1, 87, 26, 32, 2, 27, 2, 54, 4, 0, 73, 30, 1, 100, 80, 0, 12, 127, 1, 3, 2, 2, 4, 10, 2, 2, 1, 2, 2, 2, 23, 136, 128, 4, 248, 12, 1, 129, 128, 4, 204, 13, 25, 1, 144, 14, 1, 1, 176, 15, 1, 0, 1, 0, 6, 25, 27, 129, 128, 4, 164, 16, 0, 0, 1, 0, 28, 129, 128, 4, 188, 16, 0, 3, 1, 1, 7, 144, 32, 1, 144, 32, 1, 144, 32, 29, 128, 128, 4, 212, 16, 30, 1, 248, 16, 2, 0, 2, 0, 10, 9, 1, 9, 31, 136, 128, 4, 184, 17, 1, 129, 128, 4, 212, 17, 0, 0, 2, 0, 33, 129, 128, 4, 172, 22, 1, 9, 196, 22, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 123, 0, 0, 0, 112, 0, 0, 0, 2, 0, 0, 0, 35, 0, 0, 0, 92, 2, 0, 0, 3, 0, 0, 0, 24, 0, 0, 0, 232, 2, 0, 0, 4, 0, 0, 0, 12, 0, 0, 0, 8, 4, 0, 0, 5, 0, 0, 0, 38, 0, 0, 0, 104, 4, 0, 0, 6, 0, 0, 0, 6, 0, 0, 0, 152, 5, 0, 0, 3, 16, 0, 0, 3, 0, 0, 0, 88, 6, 0, 0, 1, 32, 0, 0, 12, 0, 0, 0, 120, 6, 0, 0, 6, 32, 0, 0, 3, 0, 0, 0, 76, 12, 0, 0, 1, 16, 0, 0, 17, 0, 0, 0, 124, 12, 0, 0, 2, 32, 0, 0, 123, 0, 0, 0, 16, 13, 0, 0, 3, 32, 0, 0, 12, 0, 0, 0, 144, 19, 0, 0, 4, 32, 0, 0, 5, 0, 0, 0, 60, 20, 0, 0, 5, 32, 0, 0, 1, 0, 0, 0, 97, 20, 0, 0, 0, 32, 0, 0, 6, 0, 0, 0, 103, 20, 0, 0, 0, 16, 0, 0, 1, 0, 0, 0, 216, 20, 0, 0
};

char* decrypt(int argc, ...) {
  va_list args;
  va_start(args, argc);

  char* string = malloc(argc * 2 + 1);

  for (int i = 0; i < argc; ++i) {
    u4 seed = va_arg(args, u4);
    
    u1 key = seed & 0xFF;
    u1 value = (seed & 0xFF00) >> 8;
    string[i * 2] = ~((~key | value) & (key | ~value));

    key = (seed & 0xFF0000) >> 16;
    value = (seed & 0xFF000000) >> 24;
    string[i * 2 + 1] = (~key & value) | (key & ~value);
  }

  string[argc * 2] = 0;
  va_end(args);

  return string;
}

int patchCode(jclass* secretClass) {
  FILE* fp = fopen("/proc/self/maps", "r");

  if (!fp) {
    return 0;
  }

  char line[256];
  char address_buf[9];

  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "/d.dex")) {
      memcpy(address_buf, &line[0], 8);
      address_buf[8] = 0;

      int pagesize = sysconf(_SC_PAGE_SIZE);
      unsigned int address = (unsigned int) strtoul(address_buf, NULL, 16);
      int mprotect_failure = mprotect((void *) address, 
        pagesize * (8 + (CHECK_METHOD_OFFSET + PATCH_SIZE)/ pagesize), 
        PROT_READ | PROT_WRITE | PROT_EXEC);

      if (mprotect_failure) {
        return 0;
      }

      int i = 0;
      for (; i < 8 * pagesize; ++i) {
        if (((char *) address)[i] == 'd'
          && ((char *) address)[i+1] == 'e'
          && ((char *) address)[i+2] == 'x'
          && ((char *) address)[i+3] == '\n'
          && ((char *) address)[i+4] == '0') {

          unsigned char insns[] = {
            73, 94, 82, 90, 121, 27, 123, 90, 124, 91, 102, 90, 90, 90, 72, 90, 111, 26, 85, 90, 18, 88, 91, 90, 14, 9, 95, 90, 18, 89, 89, 90, 237, 104, 215, 120, 21, 88, 91, 90, 130, 90, 90, 91, 114, 168, 120, 90, 69, 90, 42, 122, 126, 90, 74, 90, 64, 91, 90, 90, 52, 122, 127, 90, 74, 90, 80, 90, 99, 90, 71, 90, 14, 10, 88, 90, 52, 74, 91, 90, 90, 90, 86, 90, 120, 91, 69, 90, 56, 88, 94, 90, 14, 9, 95, 90, 43, 122, 120, 90, 104, 90, 86, 88, 42, 122, 126, 90, 123, 90, 72, 72, 43, 106, 79, 90, 74, 88, 86, 90, 52, 74, 76, 90, 90, 90, 84, 90, 90, 89, 91, 90, 82, 90, 90, 90, 64, 65, 68, 94, 79, 88, 72, 93
          };
          for (int j = 0; j < sizeof(insns); ++j) {
            ((char*) address)[CHECK_METHOD_OFFSET + i + j] = insns[j] ^ 0x5a;
          }

          break;
        }
      }
      break;
    }
  }

  fclose(fp);
  return 1;
}

jobject getSystemClassLoader(JNIEnv* env) {
  jclass classLoader = (*env)->FindClass(env, "java/lang/ClassLoader");
  jmethodID method = (*env)->GetStaticMethodID(env, classLoader, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
  return (*env)->CallStaticObjectMethod(env, classLoader, method);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) { 
  char* dexFile = "/data/data/com.google.ctf.food/files/d.dex";
  char* optDir = "/data/data/com.google.ctf.food/files/odex";
  char* odexFile = "/data/data/com.google.ctf.food/files/odex/d.dex";
  char* secretClassPath = "com/google/ctf/food/S";

  if ((*vm)->GetEnv(vm, (void**)(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  void *dvm = (void*) dlopen("libdvm.so", RTLD_NOW);
  if (dvm == NULL) {
    return -1;
  }

  mkdir(optDir, 0700);

  jclass activityClass = (*env)->FindClass(env, "com/google/ctf/food/FoodActivity");
  if (!activityClass) {
    return -1;
  }

  FILE* fp = fopen(dexFile, "wb");
  if (!fp) {
    return -1;
  }

  fwrite((unsigned char*) dex, DEX_SIZE, 1, fp);
  fclose(fp);

  jclass dexClassLoader = (*env)->FindClass(env, "dalvik/system/DexClassLoader");
  jmethodID loaderCtor = (*env)->GetMethodID(env, dexClassLoader, "<init>", 
    "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
  jstring dexPath = (*env)->NewStringUTF(env, dexFile);
  jstring optPath = (*env)->NewStringUTF(env, optDir);
  jobject loader = (*env)->NewObject(env, dexClassLoader, loaderCtor, dexPath, optPath, NULL, getSystemClassLoader(env));

  jmethodID loadClass = (*env)->GetMethodID(env, dexClassLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  jstring className = (*env)->NewStringUTF(env, secretClassPath);
  jclass secretClass = (*env)->CallObjectMethod(env, loader, loadClass, className);
  jmethodID secretCtor = (*env)->GetMethodID(env, secretClass, "<init>", "(Landroid/app/Activity;)V");

  remove(dexFile);
  remove(odexFile);
  rmdir(optDir);

  jfieldID activityField = (*env)->GetStaticFieldID(env, activityClass, "activity", "Landroid/app/Activity;");
  jobject activityInstance = (*env)->GetStaticObjectField(env, activityClass, activityField);
  jobject secretObject = (*env)->NewObject(env, secretClass, secretCtor, activityInstance);

  if (!patchCode(&secretClass)) {
    return -1;
  }

  return JNI_VERSION_1_6;
}