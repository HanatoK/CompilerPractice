#!/usr/bin/env python3

word_list = []
with open('reserved_words', 'r') as fInput:
    for line in fInput:
        if line:
            for field in line.split(','):
                word = field.strip()
                if word:
                    word_list.append(word)

for word in word_list:
    lower_case = word.lower()
    # print(f'mReservedWordsMap["{lower_case}"] = PascalTokenType::{word};')
    print(f'reservedWordsMap[PascalTokenType::{word}] = QString("{lower_case}");')