import os
from os import listdir
import re
import hashlib
import subprocess
import sys
import string
import csv
import traceback

pre = ['bac',
       '31254',
       'ABCDEFG',
       'float',
       'tlLoac']

inorder = ['abc',
       '12345',
       'CBDAFEG',
       'float',
       'Llotac']

sol = ['acb',
       '21453',
       'CDBFGEA',
       'taolf',
       'Lolcat']

hashes = set()
duplicatehashes = set()
results = [["NetId","Grade","Comment"]]

BINARY="testbinarytree"
CODE=["main.c", "treeTraversal.c", "treeTraversal.h"]

def populate_hashes(filepath):
    hashval = hashlib.md5(open(filepath, 'rb').read()).hexdigest()
    if (hashval not in hashes):
        hashes.add(hashval)
    else:
        duplicatehashes.add(hashval)

def check_binary(path, score, comment):
    valid = True
    binaryfilepath = path+"/testbinarytree"
    output=subprocess.check_output("file '{}'".format(binaryfilepath),shell=True).decode()
    hdr='{}: '.format(binaryfilepath)
    if (output.startswith(hdr) and output.endswith('\n') and ("executable" in output)) != True:
        score = score - 8
        comment = comment + "[-8] Invalid binary; "
    # print output
    return (score, comment)

# Deduct a maximum of 17 (-17 for runtime error, -12 if all incorrect outputs (either all pals, all non-pals or both), -2 if error with valgrind)
def check_output (fpath, score, comment):
    correct = True
    rt_error = False
    read_string_error = False
    valgrind_err = False
    failed_pals = []
    failed_nonpals = []
    valgrind_check = False
    valgrind_cmd = " valgrind --leak-check=full "
    valgrind_cmd = " "
#    valgrind_cmd = ""
    SCRIPT_PATH = os.path.dirname(os.path.realpath(__file__))
    if os.geteuid() == 0:
        subprocess.check_output('chmod -R a+rx ' + fpath, stderr = subprocess.STDOUT, shell=True)
    test_dir = os.path.join(SCRIPT_PATH, "tests_lite")
    try:
        for testcase in range(0,5):
            testfile = os.path.join(test_dir, "test" + str(testcase) + '.txt')
            # Note: start process as nobody for a bit of security/isolation
            # if the autograder is run as root. To remove, delete "sudo -u nobody"
            # This call does not quote paths right now!
            # If spaces are in a path, the command may fail.
            if os.geteuid() == 0:
                prefix = "sudo -u nobody "
            else:
                prefix = ""
            runoutput = subprocess.check_output(prefix + 'timeout 2 ' + valgrind_cmd + fpath + '/graderbinary < ' + testfile, stderr = subprocess.STDOUT, shell=True).decode("utf-8")
            # if (sol[testcase] not in runoutput):
            solfile = os.path.join(test_dir, "test" + str(testcase) + '.sol')
            with open(solfile, "rb") as f:
                expected = f.readline()
                if expected != runoutput:
                    correct = False
                    failed_pals.append((pre[testcase] +' - '+inorder[testcase]))
            if (valgrind_check and ("ERROR SUMMARY: 0 error" not in runoutput)):
                # print "***********" + runoutput + "****"
                valgrind_err = True

            # run valgrind only once
            valgrind_check = False
            valgrind_cmd = ""

    except subprocess.CalledProcessError:
        rt_error = True
    except UnicodeDecodeError:
        read_string_error = True

    if rt_error:
        score -= 17
        if sys.exc_info()[1].returncode == 124:
            comment += "[-17] program timed out (infinite loop?)"
        else:
            comment += "[-17] Runtime error: " + sys.exc_info()[1].output.decode("utf-8")
    elif read_string_error:
        score -= 17
        comment += "[-17] Runtime error: Invalid string stored in n."
    elif not correct:
        if len(failed_pals) == len(sol):
            # if all cases of a particular type fail
            deduct = 12
        else:
            deduct = 1+11 * (len(failed_pals)+len(failed_nonpals))/(len(sol))
        score -= deduct
        comment += "[-" + `deduct` + "] Failed cases: " + string.join(failed_pals+failed_nonpals, ", " + "; ")
    if ((not (rt_error or read_string_error)) and valgrind_err):
        score -= 2
        comment += "[-2] valgrind leak-check failed."

    comment += "; "
    return (score, comment)

# Deduct a maximum of 22 (-22, if doesn't compile; 17 based on running correctly)
def compile_program(fpath, score, comment):
    try:
        invalid_format = False
        for src in CODE:
            output=subprocess.check_output("file '{}'".format(fpath+"/"+src),shell=True).decode()
            if ("C source" not in output and "c program" not in output):
                comment += '[-27] Invalid submission. %s : %s' % (src, output)
                if not invalid_format:
                    score -= 27
                invalid_format = True
        if not invalid_format:
            # compileoutput = subprocess.check_output("gcc -Wall -std=c99 " + fpath + "/" + CODE + " -o " + fpath + "/graderbinary", stderr = subprocess.STDOUT, shell=True).decode("utf-8")
            compileoutput = subprocess.check_output("make autograder_lite_binary", stderr = subprocess.STDOUT, shell=True).decode("utf-8")
            score, comment = check_output(fpath, score, comment)
            subprocess.call("rm -f " + fpath + "/graderbinary", shell=True)
    except subprocess.CalledProcessError:
        score -= (5 + 3 + 12 + 2)
        comment += '[-22] Compile error: ' + sys.exc_info()[1].output.decode("utf-8") + '; '
    return (score, comment)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "-ERROR: Missing arugment(submission directory, which should contain %s)" % (BINARY + " " + " ".join(CODE))
        print "-Usage: python %s SUBMISSIONS_DIRECTORY" % sys.argv[0]
        sys.exit(1)
    SUBMISSIONS_DIR = sys.argv[1]
    score = 35
    comment = " [ C-Programming: "
    submissions = os.listdir(SUBMISSIONS_DIR)
    if BINARY in submissions:
        score, comment = check_binary(SUBMISSIONS_DIR, score, comment)
    else:
        score = score - 8
        comment = comment + "[ERROR] No binary submitted; "
    has_c_source = True
    for i in CODE:
        if i not in submissions:
            has_c_source = False
            score = score - 27
            comment = comment + "[ERROR] No C code %s submitted; " % i
    if has_c_source:
        score, comment = compile_program(SUBMISSIONS_DIR, score, comment)

    if score == 35:
        comment += "Great job!"
    elif score >= 28:
        comment += "Good job!"
    comment += " ] "
#     with open(sys.argv[2], 'wb') as result_file:
    print comment.encode('utf-8').strip()
        # result_file.write(comment.encode('utf-8').strip())
        # result_file.write("\n")
