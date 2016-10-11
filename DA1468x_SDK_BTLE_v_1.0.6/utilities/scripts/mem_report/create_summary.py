#########################################################################################
# Copyright (C) 2016. Dialog Semiconductor Ltd, unpublished work. This computer
# program includes Confidential, Proprietary Information and is a Trade Secret of
# Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
# unless authorized in writing. All Rights Reserved.
#
# <black.orca.support@diasemi.com> and contributors.
#########################################################################################

import sys
import os
import map_reader
import xlxcreator


def get_prj_names(full_path):
    c = os.path.basename(os.path.dirname(full_path))
    p = os.path.splitext(os.path.basename(full_path))[0]
    return p, c


def adjust_maximum_name_size(current_maximum_name_size, p_name, c_name):
    if len(p_name + "/" + c_name) > current_maximum_name_size:
        return len(p_name + "/" + c_name)
    else:
        return current_maximum_name_size


def get_main_sizes(map_file):
    text_sections = [".text", ".ARM.exidx", "jump_table_mem_area", "RETENTION_ROM0",
                     ".default_patch_code_handler_section"]
    data_sections = [".data", ".copy.table", ".zero.table"]
    bss_sections = [".bss", "RETENTION_RAM0", "RETENTION_RAM1", "RETENTION_BLE"]
    ignore_sections = [".heap", ".stack_dummy"]
    section_data = map_reader.generate_report(map_file)
    text_size = sum([int(section_data[sec]["size"], 16) for sec in section_data if sec in text_sections])
    data_size = sum([int(section_data[sec]["size"], 16) for sec in section_data if sec in data_sections])
    bss_size = sum([int(section_data[sec]["size"], 16) for sec in section_data if sec in bss_sections])
    retained_size = sum([int(section_data[sec]["size"], 16) for sec in section_data if "RETENTION" in sec])
    unknown = [sec for sec in section_data if
               sec not in text_sections and sec not in data_sections and sec not in bss_sections and
               sec not in ignore_sections and int(section_data[sec]["size"], 16) > 0]

    if len(unknown) > 0:
        print "WARNING, unknown sections discovered in %s" % map_file
        print unknown

    return text_size, data_size, bss_size, (text_size + data_size), retained_size

search_path = os.path.abspath(sys.argv[1])
report_name = sys.argv[2]

# Start Excel file creation
E = xlxcreator.XLwithXlswriter(report_name)

# Create Main worksheet
E.create_sheet("Projects Summary")

# Write main title
title_data = [["Project [Configuration]", "code", "data", "bss", "image size", "total retained RAM"]]
E.write_worksheet("Projects Summary", 0, 0, title_data, "main row title")

maximum_name_size = len(title_data[0][0])

# Write data
active_row = 1
for root, dirs, files in os.walk(search_path):
    for f in files:
        if f.endswith(".map"):
            project, configuration = get_prj_names(os.path.join(root, f))
            maximum_name_size = adjust_maximum_name_size(maximum_name_size, project, configuration)
            E.write_worksheet("Projects Summary", active_row, 0, [[project + " [" + configuration + "]"]],
                              "main column title")
            code, data, bss, img, ret_ram = get_main_sizes(os.path.join(root, f))
            E.write_worksheet("Projects Summary", active_row, 1, [[code, data, bss]], "data")
            E.write_worksheet("Projects Summary", active_row, 4, [[img, ret_ram]], "data total")
            active_row += 1

# Set column widths
E.set_column("Projects Summary", 0, maximum_name_size + 2)
E.set_column("Projects Summary", 1, 10)
E.set_column("Projects Summary", 2, 10)
E.set_column("Projects Summary", 3, 10)
E.set_column("Projects Summary", 4, 10)
E.set_column("Projects Summary", 5, 20)

# Add an auto-filter in project name column to allow filtering out some
E.add_drop_down_selector("Projects Summary", 0, 0, active_row - 1, 0)

# Add comments
E.add_comment("Projects Summary", 0, 1, "The size of the '.text' section as given by the GNU size utility.\n\n"
                                        "For the SDK projects this is the sum of the following sections:\n\n"
                                        "'.text'\n'.ARM.exidx'\n'jump_table_mem_area'\n'RETENTION_ROM0'\n"
                                        "'.default_patch_code_handler_section'")
E.add_comment("Projects Summary", 0, 2, "The size of the '.data' section as given by the GNU size utility.\n\n"
                                        "For the SDK projects this is the sum of the following sections:\n\n"
                                        "'.data'\n'.copy.table'\n'zero.table'")
E.add_comment("Projects Summary", 0, 3, "The size of the '.bss' section as given by the GNU size utility.\n\n"
                                        "For the SDK projects this is the sum of the following sections:\n\n"
                                        "'.bss'\n'RETENTION_RAM0'\n'RETENTION_RAM1'\n'RETENTION_BLE'")
E.add_comment("Projects Summary", 0, 4, "The size of the project in Flash (without counting additional header bytes "
                                        "by the booting procedure). Essentially the sum of \"code\" and \"data\".")
E.add_comment("Projects Summary", 0, 5, "The size of all sections that end up in retained RAM.\n\n"
                                        "For the SDK projects this is the sum of the sections that have (guess what...)"
                                        " \"RETENTION\" in their name:\n\n"
                                        "'.RETENTION_ROM0'\n'RETENTION_RAM0'\n'RETENTION_RAM1'\n'RETENTION_BLE'")

# Close Excel file
E.close_workbook()

