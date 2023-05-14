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
    parser.add_argument('input', help='input dot file name', type=str)
    parser.add_argument('-o', help='output files basename (def. tmp.part)', default = "tmp.part", type=str)
    parser.add_argument('--DFA', help='take in input an arbitrary DFA (def. False)', action='store_true')
    parser.add_argument('--idbase1', help='activate if initial state is labelled with 1 (def. False)', action='store_true')
    parser.add_argument('--intfile', help='take in input an intermediate file (def. False)', action='store_true')
    parser.add_argument('--check', help='check output correctness (def. False)', action='store_true')
    parser.add_argument('--verbose',  help='verbose mode on (def. False)',action='store_true')
    args = parser.parse_args()

    logfile_name = args.input + ".log"
    # get main directory
    args.main_dir = os.path.split(sys.argv[0])[0]
    print("Sending logging messages to file:", logfile_name)

    command = "gc {input} -n".format(input=args.input)
    print("Counting number of nodes in " + args.input + " file...")
    out = subprocess.check_output(command.split())
    no_nodes = int(out.split()[0])
    
    with open(logfile_name,"a") as logfile:

        start0 = start = time.time()

        if(no_nodes < 2**32-1):

            print("Running in 32 bit mode.")
            if(not args.verbose):
                command = "{exe} {file} {ofile} {nodes}".format(
                        exe = os.path.join(args.main_dir,partref32_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes)
            else:
                command = "{exe} {file} {ofile} {nodes}".format(
                        exe = os.path.join(args.main_dir,partref32_verb_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes)     

        else:

            print("Running in 64 bit mode.")
            if(not args.verbose):
                command = "{exe} {file} {ofile} {nodes}".format(
                        exe = os.path.join(args.main_dir,partref64_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes)
            else:
                command = "{exe} {file} {ofile} {nodes}".format(
                        exe = os.path.join(args.main_dir,partref64_verb_exe),
                        file=args.input, ofile = args.o, nodes=no_nodes)

        if(args.idbase1):
            command += " 1"
        else:
            command += " 0"

        if(args.DFA):
            command += " 1"
        else:
            command += " 0"

        if(args.intfile):
            command += " 0"
        else:
            command += " 1"

        command += " 1"

        if( args.DFA ):
            print("==== sorting NFA. Command: ", command)
        else:
            print("==== sorting DFA. Command: ", command)

        if(execute_command(command,logfile,logfile_name)!=True):
            return
        print("Elapsed time: {0:.4f}".format(time.time()-start))

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