import run
from glob import glob
from os.path import basename


input_files = glob("instances/*.gr")
medium_real = glob("instances/medium_real/*.edges")
real_networks = []
real_networks += medium_real
input_files += real_networks
girgs = glob("instances/girg/*.edges")
input_files += girgs


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
        "output_file": lambda args: f"instances/girg/scale{args['n']}_{args['ple']}_{args['temp']}_{args['deg']}_{args['sample']}.edges",
        "n": [400*i**2 for i in range(1,17)],
        "ple": [2.1,2.7,10],
        "deg": [10],
        "temp": [0, 0.1, 0.75],
        "sample": [1,2,3,4,5],
    },
    creates_file="[[output_file]]"
)

run.group("stats")

run.add(
    "stats",
    "build/cli/stats --noheader [[input_files]]",
    {"input_files": input_files},
    stdout_file="output/stats.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
    header_command='echo "graph,"$(build/cli/stats /dev/null --onlyheader)',
)

run.add(
    "degree",
    "build/cli/degree --noheader [[input_files]]",
    {"input_files": input_files},
    stdout_file=lambda args: f"output/degree/{basename(args['input_files'])}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
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
    "expansion",
    "build/cli/expan -s 250 --noheader --alpha [[alpha]] --base [[base]] [[input_files]]",
    {
        "input_files": input_files,
        "alpha": ["0,0.1,0.25,0.5,0.66"],
        "base": [1.1,1.25,1.5,2,4],
    },
    stdout_file=lambda args: f"output/expan/{basename(args['input_files'])}_alpha={args['alpha']}_b={args['base']}.csv",
    stdout_res=lambda args: prepend_graph(args["stdout"], args["input_files"]),
)

run.add(
    "rho_expansion",
    "build/cli/expan -s 250 --noheader --rhoeval --base [[base]] [[input_files]]",
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
    {"algo": ["bfs", "degree", "expan"]}
)
run.add(
    "merge_rho_expan",
    "scripts/merge.sh 'expan --rhoeval' graph rho_expan",
    {}
)


run.group("clean")
run.add(
    "remove_merged_[[algo]]",
    "rm output/[[algo]]_merged.csv",
    {"algo": ["bfs", "degree", "expan"]},
)


run.use_cores(12)
run.run()
