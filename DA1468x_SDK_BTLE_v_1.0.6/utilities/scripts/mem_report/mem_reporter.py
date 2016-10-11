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

args = map_reader.parse_args()

if map_reader.map_file_exists(args.map_file) is False:
    print "Error while parsing arguments"
    sys.exit(1)

# Read the map file and generate section data
section_data = map_reader.generate_report(args.map_file)

# Get module data
seen_sections, module_data = map_reader.analyse_modules(section_data)

# Generate Excel file name
fp = os.path.abspath(args.map_file)
file_name_post = os.path.splitext(os.path.basename(fp))[0] + '.xlsx'
file_name_pre = os.path.basename(os.path.dirname(fp))
xl_name = file_name_pre + '_' + file_name_post

# Start Excel file creation
E = xlxcreator.XLwithXlswriter(xl_name)

# Create Main (Summary) worksheet
E.create_sheet("Summary")

# Create a worksheet for each module
for module in module_data:
    E.create_sheet(module)

# Write title for each module worksheet and arrange its width
ws_data = [seen_sections + ["total", "filename"]]
for module in module_data:
    E.write_worksheet(module, 0, 0, ws_data, "row title")
    for name in ws_data[0]:
        E.set_column(module, ws_data[0].index(name), len(name) + 2)

# Write data for each module
for module in module_data:
    formatted_object_data = map_reader.format_module_data(seen_sections, module_data[module])
    # First the numbers
    ws_data = [formatted_object_data[o] for o in formatted_object_data]
    E.write_worksheet(module, 1, 0, ws_data, "data")
    # Then the object file names
    ws_data = [[o] for o in formatted_object_data]
    E.write_worksheet(module, 1, len(seen_sections) + 1, ws_data, "data string")
    E.set_column(module, len(seen_sections) + 1, len(max([s[0] for s in ws_data], key=len)))
    # Then the vertical (column) totals
    ws_data = [['=SUM(' + xlxcreator.cell_id(r, 0) + ':' + xlxcreator.cell_id(r, len(seen_sections) - 1) + ')']
               for r in range(1, len(formatted_object_data) + 1)]
    E.write_worksheet(module, 1, len(seen_sections), ws_data, "data total")
    # And finally the horizontal (row) totals
    ws_data = [['=SUM(' + xlxcreator.cell_id(1, c) + ':' + xlxcreator.cell_id(len(formatted_object_data), c) + ')'
                for c in range(0, len(seen_sections) + 1)]]
    ws_data[0] += ["TOTAL"]
    E.write_worksheet(module, len(formatted_object_data) + 1, 0, ws_data, "data total")

# Now let's write the summary page

# First we write the horizontal title
additional_sections = list(set(map_reader.get_non_zero_sections(section_data)) - set(seen_sections))
ws_data = [seen_sections + additional_sections]
E.write_worksheet("Summary", 0, 1, ws_data, "main row title")
for name in ws_data[0]:
    E.set_column("Summary", ws_data[0].index(name) + 1, len(name) + 4)

# Then the vertical title
ws_data = [[m] for m in module_data]
E.write_worksheet("Summary", 1, 0, ws_data, "main column title")
E.set_column("Summary", 0, len(max([s[0] for s in ws_data], key=len)))

# Then the main data taken from the modules
for module in module_data:
    formatted_object_data = map_reader.format_module_data(seen_sections, module_data[module])
    ws_data = [['=' + module + '!' + xlxcreator.cell_id(len(formatted_object_data) + 1, c)
                for c in range(0, len(seen_sections))]]
    E.write_worksheet("Summary", list(module_data).index(module) + 1, 1, ws_data, "main data")

# Now the data from sections that are not defined in modules
ws_data = [[0, 0] for i in range(len(module_data))]
E.write_worksheet("Summary", 1, len(seen_sections) + 1, ws_data, "main data")

# And then the totals of the above
ws_data = [["TOTAL"] +
           ['=SUM(' + xlxcreator.cell_id(1, c) + ':' + xlxcreator.cell_id(len(module_data), c) + ')'
            for c in range(1, len(seen_sections) + 1)]]
E.write_worksheet("Summary", len(module_data) + 1, 0, ws_data, "main data total")
ws_data = [list(map(lambda x: int(section_data[x]["size"], 16), additional_sections))]
E.write_worksheet("Summary", len(module_data) + 1, len(seen_sections) + 1, ws_data, "main data total")

# Now let's sum up retention usage to create a nice graph
ws_data = [["Retention total"]]
E.write_worksheet("Summary", 0, len(seen_sections) + len(additional_sections) + 1, ws_data, "main row title")
E.set_column("Summary", len(seen_sections) + len(additional_sections) + 1, len(ws_data[0][0]))
col_id_of_retention_sections = [seen_sections.index(s) + 1 for s in seen_sections if "RETENTION" in s]
ws_data = [['=SUM(' + ','.join([xlxcreator.cell_id(r, c) for c in col_id_of_retention_sections]) + ')']
           for r in range(1, len(module_data) + 2)]
E.write_worksheet("Summary", 1, len(seen_sections) + len(additional_sections) + 1, ws_data, "main data total")

# Finally let's add the graph
retention_budget_graph_config = {'type': 'pie'}
E.create_chart("Retention Budget", retention_budget_graph_config)
retention_budget_data_config = {
    'values': ['Summary', 1, len(seen_sections) + len(additional_sections) + 1,
               len(module_data), len(seen_sections) + len(additional_sections) + 1],
    'categories': ['Summary', 1, 0, len(module_data), 0],
    'name': ['Summary', 0, len(seen_sections) + len(additional_sections) + 1],
    'data_labels': {'percentage': True, 'position': 'outside_end', 'leader_lines': True},
}
E.add_chart_data_series("Retention Budget", retention_budget_data_config)
E.set_chart_size("Retention Budget", 2.5, 2.5)
E.add_chart_to_sheet("Summary", "Retention Budget", xlxcreator.cell_id(len(module_data) + 5, 1))

# Close Excel file
E.close_workbook()
