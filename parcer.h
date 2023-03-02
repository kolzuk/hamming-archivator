#include "HamArc.h"
#include "Hamming_Codding.h"

#include <string>
#include <iostream>
#include <vector>

bool is_list_in = false;
bool is_new_archive = false;
bool is_append_file = false;
bool is_extract_file = false;
bool is_extract_all = false;
bool is_delete_file = false;

char* append_filename;
char* extract_filename;
char* delete_filename;
char* first_merge_arcname;
char* second_merge_arcname;
char* final_arcname;

void ParcingArguments(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        std::string option = argv[i];

        if (option == "-c" || option == "--create") {
            is_new_archive = true;
        } else if (option == "-f") {
            HamArc Arc(argv[i + 1]);
            if (is_append_file && !is_new_archive)
                Arc.AppendFile(append_filename);
            if (is_delete_file && !is_new_archive)
                Arc.DeleteFile(delete_filename);
            if (is_extract_file && !is_new_archive)
                Arc.ExtractFile(extract_filename);
            if (is_extract_all && !is_new_archive)
                Arc.ExtractAll();
            if (is_list_in && !is_new_archive)
                Arc.ShowFiles();
        } else if (option[0] == '-' && option[1] == '-' && option[2] == 'f' && option[3] == 'i' && option[4] == 'l' && option[5] == 'e' && option[6] == '=') {
            std::string archiveName;
            for (int j = 7; j < sizeof(option) / sizeof(char); j++)
                archiveName += option[j];
            char* chars = new char [archiveName.size() + 1];
            archiveName.copy(chars, archiveName.size());
            chars[archiveName.length()] = '\0';
            HamArc Arc(chars);
            for (int j = i + 1; j < argc; j++)
                Arc.AppendFile(argv[j]);
            if (is_append_file)
                Arc.AppendFile(append_filename);
            if (is_delete_file)
                Arc.DeleteFile(delete_filename);
            if (is_extract_file)
                Arc.ExtractFile(extract_filename);
            if (is_extract_all)
                Arc.ExtractAll();
            if (is_list_in)
                Arc.ShowFiles();
        } else if (option == "-a" || option == "--append") {
            is_append_file = true;
            append_filename = argv[i + 1];
        } else if (option == "-x" || option == "--extract") {
            extract_filename = argv[i + 1];
            std::ifstream file(extract_filename, std::ios_base::binary);
            if (!file.is_open()) {
                is_extract_all = true;
            } else {
                is_extract_file = true;
            }
        } else if (option == "-l" || option == "--list") {
            is_list_in = true;
        } else if (option == "-d" || option == "--delete") {
            is_delete_file = true;
            delete_filename = argv[i + 1];
        } else if (option == "--code") {
            uint64_t codingLength = atoi(argv[i + 1]);
            Coding(argv[i + 2], codingLength);
        } else if (option == "--decode") {
            Decoding(argv[i + 1]);
        } else if (option == "-A" || option == "--concatenate") {
            first_merge_arcname = argv[i + 1];
            second_merge_arcname = argv[i + 2];
            final_arcname = argv[i + 3];
            MergeArc(first_merge_arcname, second_merge_arcname, final_arcname);
        }
    }
}