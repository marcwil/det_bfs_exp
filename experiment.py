import run
from glob import glob
from os.path import basename



input_files = glob("input/*.gr")

def prepend_graph(output, graph):
    return "\n".join([f"{basename(graph)},{line}" for line in output.splitlines()])

run.group("Stats")

run.add(
    "stats",
    "build/cli/stats --noheader [[input_files]]",
    {'input_files': input_files},
    stdout_file="output/stats.csv",
    stdout_res=lambda args: prepend_graph(args['stdout'], args['input_files']),
    header_command="echo \"graph,\"$(build/cli/stats /dev/null --onlyheader)",
)
 

run.group("BiBFS")

run.add(
    "bibfs_cost",
    "build/cli/bfs -s 100 --noheader [[input_files]]",
    {'input_files': input_files},
    stdout_file=lambda args: f"output/bfs/{basename(args['input_files'])}.csv",
    stdout_res=lambda args: prepend_graph(args['stdout'], args['input_files']),
)


run.group("Postprocessing")

run.add(
    "merge_csvs",
    "cat output/bfs/*.csv",
    {'name': ['bfs']},
    header_command="echo \"graph,\"$(build/cli/bfs /dev/null --onlyheader)",
    stdout_file="output/bfs_merged.csv"
)

run.use_cores(4)
run.run()