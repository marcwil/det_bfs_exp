import run
from glob import glob
from os.path import basename
from os import environ
import os

num_cores = environ.get("NUM_CORES")
if num_cores is None:
    num_cores = 1
elif num_cores == "all":
    num_cores = len(os.sched_getaffinity(0))
else:
    num_cores = int(num_cores)
print(f"Using {num_cores} cores")

def prepend_graph(output, graph):
    return "\n".join([f"{basename(graph)},{line}" for line in output.splitlines()])


run.group("gengraph")

run.add(
    "girg_range",
    "build/cli/graph_generator -o [[output_file]] -n [[n]] --ple [[ple]] --deg [[deg]] --temp [[temp]]",
    {
        "output_file": lambda args: f"instances/girg/girg_{args['n']}_{args['ple']}_{args['temp']}_{args['deg']}_{args['sample']}.edges",
        "n": [50000],
        "ple": [2.1,2.3,2.5,3,5,10],
        "deg": [10],
        "temp": [0, 0.1, 0.2, 0.5, 0.75, 0.99],
        "sample": [1,2,3,4,5],
    },
    creates_file="[[output_file]]",
)

run.add(
    "girg_scale",
    "build/cli/graph_generator -o [[output_file]] -n [[n]] --ple [[ple]] --deg [[deg]] --temp [[temp]]",
    {
        "output_file": lambda args: f"instances/scale_girg/scale{args['n']}_{args['ple']}_{args['temp']}_{args['deg']}_{args['sample']}.edges",
        "n": [round(300*1.5**i) for i in range(1,23)],
        "ple": [2.1,2.3,2.5,2.7,3, 3.5,6,10],
        "deg": [10],
        "temp": [0],
        "sample": [1,2,3,4,5],
    },
    creates_file="[[output_file]]"
)
run.use_cores(num_cores)
run.run()

input_files = glob("instances/*.gr")
all_real = glob("instances/edge_lists_real/*")
real_networks = []
real_networks += all_real

input_files += real_networks

girgs = glob("instances/girg/girg*.edges")
#input_files += girgs
scale_girgs = glob("instances/scale_girg/*.edges")
#input_files += scale_girgs

print(f"Input files: {len(input_files)}, scale girgs: {len(scale_girgs)}, total: {len(input_files)+len(scale_girgs)}")


run.group("stats")

run.add(
    "stats",
    "build/cli/stats --noheader [[input_files]]",
    {"input_files": input_files + scale_girgs},
    stdout_file="output/stats/[[input_files]].csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
#    header_command='echo "graph,"$(build/cli/stats /dev/null --onlyheader)',
)

run.add(
    "teex_diameter",
    "python ./scripts/teexwrap.py ./ext_libs/teexgraph/build/teexgraph [[input_files]] -s 1",
    {"input_files": input_files},
    stdout_file=lambda args: f"output/teex_diameter/{basename(args['input_files'])}.csv",
    #stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)


run.group("experiments")

run.add(
    "bibfs_cost",
    "build/cli/bfs -s 250 --noheader [[input_files]]",
    {"input_files": input_files},
    stdout_file=lambda args: f"output/bfs/{basename(args['input_files'])}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)

run.add(
    "bibfs_perf",
    "build/cli/perftest -s 1000 --noheader [[input_files]]",
    {"input_files": input_files + scale_girgs},
    stdout_file=lambda args: f"output/perftest/{basename(args['input_files'])}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)

run.add(
    "expansion",
    "build/cli/expan -s 250 --noheader --alpha [[alpha]] --base [[base]] [[input_files]]",
    {
        "input_files": input_files + scale_girgs,
        "alpha": ["0,0.05,0.1,0.2,0.3,0.4,0.5"],
        "base": [1.1,1.25,1.5,2,4],
    },
    stdout_file=lambda args: f"output/expan/{basename(args['input_files'])}_alpha={args['alpha']}_b={args['base']}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)

run.add(
    "rho_alpha_expansion",
    "build/cli/expan -s 250 --noheader --rhoeval --base [[base]] --alpha [[alpha]] [[input_files]]",
    {
        "input_files": input_files,
        "base": [1.1, 1.25, 1.5, 2, 4],
        "alpha": ["0,0.05,0.1,0.2,0.3,0.4,0.5"],
    },
    stdout_file=lambda args: f"output/rho_alpha_expan/{basename(args['input_files'])}_{args['base']}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)

run.add(
    "rho_expansion",
    "build/cli/expan -s 250 --noheader --rhoeval --optimalrho --base [[base]] [[input_files]]",
    {
        "input_files": input_files,
        "base": [1.1, 1.25, 1.5, 2, 4],
    },
    stdout_file=lambda args: f"output/rho_expan/{basename(args['input_files'])}_{args['base']}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)

run.group("post")

# merge all csv files and add header
#run.add(
#    "merge_[[algo]]",
#    "cat output/[[algo]]/*.csv",
#    {"algo": ["bfs", "degree", "expan"]},
#    header_command='echo "graph,"$(build/cli/[[algo]] /dev/null --onlyheader)',
#    stdout_file="output/[[algo]]_merged.csv"
#)

# variant of merge_[[algo]] that runs even if files already exist
run.group("merge")
run.add(
    "merge_[[algo]]",
    "scripts/merge.sh [[algo]] graph [[algo]]",
    {"algo": ["stats", "bfs", "degree", "expan", "perftest"]}
)
run.add(
    "merge_[[algo]]",
    "scripts/merge.sh 'expan --rhoeval' graph [[algo]]",
    {"algo": ["rho_expan", "rho_alpha_expan"]}
)

run.add(
    "merge_teex_diameter",
    "cat output/teex_diameter/*.csv",
    { },
    header_command='echo "graph,diameter,teex_iter,teex_time"',
    stdout_file="output/teex_diameter.csv"
)



run.group("clean")
run.add(
    "remove_merged_[[algo]]",
    "rm output/[[algo]]_merged.csv",
    {"algo": ["bfs", "degree", "expan", "perftest"]},
)


run.use_cores(num_cores)
run.run()
