#!/usr/bin/env python3

import sys, time, argparse, subprocess, os.path

Description = """
Tool to sort and prune finite automata using the partition refinement algorithm.
"""

dirname = os.path.dirname(os.path.abspath(__file__))
partref32_exe          =  os.path.join(dirname, "build/part-ref32.x")
partref64_exe          =  os.path.join(dirname, "build/part-ref64.x")
partref32_verb_exe     =  os.path.join(dirname, "build/part-ref32-verb.x")
partref64_verb_exe     =  os.path.join(dirname, "build/part-ref64-verb.x")
prune32_exe            =  os.path.join(dirname, "build/prune32.x")
prune64_exe            =  os.path.join(dirname, "build/prune64.x")
prune32_verb_exe      =  os.path.join(dirname, "build/prune32-verb.x")
prune64_verb_exe       =  os.path.join(dirname, "build/prune64-verb.x")
merge32_exe            =  os.path.join(dirname, "build/merge32.x")
merge64_exe            =  os.path.join(dirname, "build/merge64.x")
merge32_verb_exe      =  os.path.join(dirname, "build/merge32-verb.x")
merge64_verb_exe       =  os.path.join(dirname, "build/merge64-verb.x")

def main():
    parser = argparse.ArgumentParser(description=Description, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('input', help='automaton file path', type=str)
    parser.add_argument('-outpath', help='output file basename (def. filename.part)', default = "", type=str)
    parser.add_argument('--sort', help='sort a (Pseudo-)Wheeler automaton using the partition refinement algorithm (def. False)', action='store_true')
    parser.add_argument('--prune', help='compute pruned automata recognizing infima and suprema strings (def. False)', action='store_true')
    parser.add_argument('--intervals', help='compute the colex. intervals of the input automaton (def. False)', action='store_true')
    parser.add_argument('--compact', help='write the output automata in compact format (def. False)', action='store_true')
    parser.add_argument('--dot', help='take in input a dot file (def. False)', action='store_true')
    parser.add_argument('--source', help='Only for .dot inputs, define the source state of the input automaton (def. 0)', default = 0, type=int)
    parser.add_argument('--idbase1', help='activate if state ids start from 1 rather than from 0 (def. False)', action='store_true')
    parser.add_argument('--verbose',  help='verbose mode on (def. False)',action='store_true')
    args = parser.parse_args()

    logfile_name = args.input + ".log"
    # get main directory
    args.main_dir = os.path.split(sys.argv[0])[0]
    print("Sending logging messages to file:", logfile_name)
    if(args.outpath == ""):
        args.outpath = args.input

    # compute number of nodes and source state
    # for dot file we assume the source state is labelled either with 0 or 1
    no_nodes = 0
    if( args.dot ):
        command = "gc -n {input}".format(input=args.input)
        print("Counting number of nodes in " + args.input + " file...")
        out = subprocess.check_output(command.split())
        args.no_nodes = int(out.split()[0])
    else:
        with open(args.input, "rb") as file:
            header = file.readline().split()
            args.no_nodes = int(header[0])
            args.source = int(header[2])
    
    with open(logfile_name,"a") as logfile:

        start0 = start = time.time()

        if(no_nodes < 2**32-1):

            print("Running in 32 bit mode.")
            if(not args.verbose):
                args.exe = partref32_exe
                args.exe_prune = prune32_exe
                args.exe_merge = merge32_exe
            else:
                args.exe = partref32_verb_exe 
                args.exe_prune = prune32_verb_exe
                args.exe_merge = merge32_verb_exe

        else:

            print("Running in 64 bit mode.")
            if(not args.verbose):
                args.exe = partref64_exe
                args.exe_prune = prune64_exe
                args.exe_merge = merge64_exe
            else:
                args.exe = partref64_verb_exe
                args.exe_prune = prune64_verb_exe
                args.exe_merge = merge64_verb_exe

        # choose the correct algorithm #
        if args.sort:

            # standard partition refinement algorithm #
            command = "{exe} {file} {ofile} {nodes} {source} {startind} {dot} {compact}".format(
                    exe = os.path.join(args.main_dir,args.exe), file=args.input,
                    ofile = args.outpath + ".part", nodes=args.no_nodes, source=args.source,
                    startind=int(args.idbase1), dot=int(args.dot), compact=int(args.compact))

            print("==== sorting a (Pseudo)-Wheeler automaton using the partition refinement algorithm. Command: ", command)
            if(execute_command(command,logfile,logfile_name)!=True):
                return

        elif args.prune:

            # pruning algoritm - infima strings #
            command = "{exe} {file} {ofile} {nodes} {source} {dot} {startind} 0 0 {compact}".format(
                    exe = os.path.join(args.main_dir,args.exe_prune), file=args.input,
                    ofile = args.outpath + ".infima", nodes=args.no_nodes, source=args.source,
                    dot=int(args.dot), startind=int(args.idbase1), compact=int(args.compact))

            print("==== pruning infima strings automaton. Command: ", command)
            if(execute_command(command,logfile,logfile_name)!=True):
                return

            # pruning algoritm - suprema strings #
            command = "{exe} {file} {ofile} {nodes} {source} {dot} {startind} 1 1 {compact}".format(
                    exe = os.path.join(args.main_dir,args.exe_prune), file=args.input,
                    ofile = args.outpath + ".suprema", nodes=args.no_nodes, source=args.source,
                    dot=int(args.dot), startind=int(args.idbase1), compact=int(args.compact))

            print("==== pruning suprema strings automaton. Command: ", command)
            if(execute_command(command,logfile,logfile_name)!=True):
                return

        if args.intervals:

            # pruning algorithm + marging infima and suprema automata #
            command = "{exe} {file} {ofile} {nodes} {source} {dot} {startind}".format(
                    exe = os.path.join(args.main_dir,args.exe_merge), file=args.input,
                    ofile = args.outpath + ".part", nodes=args.no_nodes, source=args.source,
                    dot=int(args.dot), startind=int(args.idbase1))

            print("==== sorting a (Pseudo)-Wheeler automaton using the partition refinement algorithm. Command: ", command)
            if(execute_command(command,logfile,logfile_name)!=True):
                return

        print("Elapsed time: {0:.4f}".format(time.time()-start))

# execute command: return True is everything OK, False otherwise
def execute_command(command,logfile,logfile_name,env=None):
  try:
    #subprocess.run(command.split(),stdout=logfile,stderr=logfile,check=True,env=env)
    subprocess.check_call(command.split(),stdout=logfile,stderr=logfile,env=env)
  except subprocess.CalledProcessError:
    print("Error executing command line:")
    print("\t"+ command)
    print("Check log file: " + logfile_name)
    return False
  return True

if __name__ == '__main__':
    main()