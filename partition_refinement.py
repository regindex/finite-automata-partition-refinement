#!/usr/bin/env python3

import sys, time, argparse, subprocess, os.path

Description = """
Tool to sort finite automata using partition refinement algorithm.
"""

dirname = os.path.dirname(os.path.abspath(__file__))
partref32_exe     =  os.path.join(dirname, "build/part-ref32.x")
partref64_exe     =  os.path.join(dirname, "build/part-ref64.x")
check32_exe       =  os.path.join(dirname, "build/check32.x")
partref32_verb_exe     =  os.path.join(dirname, "build/part-ref32-verb.x")
partref64_verb_exe     =  os.path.join(dirname, "build/part-ref64-verb.x")
check32_verb_exe       =  os.path.join(dirname, "build/check32-verb.x")

def main():
    parser = argparse.ArgumentParser(description=Description, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('input', help='input automaton file name', type=str)
    parser.add_argument('-o', help='output files basename (def. filename.part)', default = "", type=str)
    parser.add_argument('--pruning', help='run the pruning algorithm (def. False)', action='store_true')
    parser.add_argument('--suprema', help='Only for --pruning, compute suprema strings pruned DFA (def. False)', action='store_true')
    parser.add_argument('--ascii', help='use the ascii alphabet (def. False)', action='store_true')
    parser.add_argument('--source', help='Only for .dot inputs, define the source state (def. 0)', default = 0, type=int)
    parser.add_argument('--idbase1', help='activate if state ids start from 1 (def. False)', action='store_true')
    parser.add_argument('--intermediate', help='take in input an intermediate file (def. False)', action='store_true')
    #parser.add_argument('--check', help='check order Wheelerness (def. False)', action='store_true')
    parser.add_argument('--verbose',  help='verbose mode on (def. False)',action='store_true')
    args = parser.parse_args()

    logfile_name = args.input + ".log"
    # get main directory
    args.main_dir = os.path.split(sys.argv[0])[0]
    print("Sending logging messages to file:", logfile_name)
    if(args.o == ""):
        if(args.pruning): args.o = args.input + ".pruned"
        else: args.o = args.input + ".part"

    # compute number of nodes and source state
    # for dot file we assume the source state is labelled either with 0 or 1
    no_nodes = 0
    if( not args.intermediate ):
        command = "gc {input} -n".format(input=args.input)
        print("Counting number of nodes in " + args.input + " file...")
        out = subprocess.check_output(command.split())
        no_nodes = int(out.split()[0])
    else:
        with open(args.input, "rb") as file:
            try:
                file.seek(-2, os.SEEK_END)
                while file.read(1) != b'\n':
                    file.seek(-2, os.SEEK_CUR) 
            except OSError:
                file.seek(0)
            last_line = file.readline().decode()
            no_nodes = int(last_line.split(" ")[0])
            args.source = int(last_line.split(" ")[2])
    
    with open(logfile_name,"a") as logfile:

        start0 = start = time.time()

        if(no_nodes < 2**32-1):

            print("Running in 32 bit mode.")
            if(not args.verbose):
                command = "{exe} {file} {ofile} {nodes} {source}".format(
                        exe = os.path.join(args.main_dir,partref32_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes,
                        source=args.source)
            else:
                command = "{exe} {file} {ofile} {nodes} {source}".format(
                        exe = os.path.join(args.main_dir,partref32_verb_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes,
                        source=args.source)     

        else:

            print("Running in 64 bit mode.")
            if(not args.verbose):
                command = "{exe} {file} {ofile} {nodes} {source}".format(
                        exe = os.path.join(args.main_dir,partref64_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes,
                        source=args.source)
            else:
                command = "{exe} {file} {ofile} {nodes} {source}".format(
                        exe = os.path.join(args.main_dir,partref64_verb_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes,
                        source=args.source)

        if(args.idbase1):
            command += " 1"
        else:
            command += " 0"

        if(args.pruning):
            command += " 1"
        else:
            command += " 0"

        if(args.suprema):
            command += " 1"
        else:
            command += " 0"

        if(args.ascii):
            command += " 1"
        else:
            command += " 0"

        if(args.intermediate):
            command += " 0"
        else:
            command += " 1"

        command += " 1"

        if( args.pruning ):
            print("==== running the partition refinement algorithm. Command: ", command)
        else:
            print("==== running the pruning algorithm. Command: ", command)

        if(execute_command(command,logfile,logfile_name)!=True):
            return
        print("Elapsed time: {0:.4f}".format(time.time()-start))
        '''
        if( args.check ):
            command = "{exe} {input} {order} {nodes}".format(
                        exe = os.path.join(args.main_dir,check32_exe),
                        input=args.input, order = args.o, nodes=no_nodes)

            if(args.idbase1):
                command += " 1"
            else:
                command += " 0"

            if(args.verbose):
                command += " 1"
            else:
                command += " 0"

            print("==== check order Wheelerness. Command: ", command)
            if(execute_command(command,logfile,logfile_name)!=True):
                return
            print("Elapsed time: {0:.4f}".format(time.time()-start))
        '''

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