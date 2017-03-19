import os, subprocess, time

# typex simulator executable
loc = "./Typex"

# ciphertext to compare
CIPHER = ('KXWCKMIWRSHTJVDJRVYYFSYYWWRZPVOROKRRNXYCVATDNGWTDOQNBRJC'
	'QPBFOOZXHSJRPSTLDMUBSUTDAQRPZEHCPFTCIYENOUTSMWBISCNLUHLA'
	'CIQPXQDNJFAOMYUNEERSZAKQJEQKKMEBFOTANYHYRFDJTVKCIGPTWCPY')
cipher_len = len(CIPHER)

# name of infile, outfile
infile = 'plain.txt'
outfile = 'cipher.txt'

# from part A - breaking the rotors
ROTORS = '427'
ORIENTATION = '100'
POS1 = 'MFV'
POS2 = 'NEV'

# other constants
ALPHA = 'ABCDEFGHIJKLMNOPQRSTUVQXYZ'
STATORS = '13568'
ORIENTATIONS = '01'

# putative settings
# stators
ls = 0
rs = 1
# orientations
lo = 0
ro = 0
# positions
lp = 0
rp = 0


# scores the outfile
def score():
	f = open(outfile)
	p_cipher = f.readline()
	# if for some reason lengths do not match
	if len(p_cipher) is not cipher_len:
		print('error with lengths', len(p_cipher), cipher_len)
		f.close()
		return -1
	score = 0
	for i in range(cipher_len):
		if p_cipher[i] is CIPHER[i]:
			# print(CIPHER[i], i)
			score += 1
	f.close()
	return score

# builds the command to run in terminal
def get_command():
	cmd_array = [
		loc,
		ROTORS + STATORS[ls] + STATORS[rs],
		ORIENTATION + ORIENTATIONS[lo] + ORIENTATIONS[ro],
		POS1 + ALPHA[lp]  + ALPHA[rp],
		infile,
		outfile
	]
	return cmd_array

# best = 0
# test_counter = 0
# best_cmd = ''

# # while(best < cipher_len):
# while(test_counter < 10):
# 	# puts a space between each argument
# 	command = ' '.join(get_command())
# 	print(command)
# 	# spawn new process
# 	result = subprocess.Popen(command, shell=True,
# 		                           stdout=subprocess.PIPE, 
# 		                           stderr=subprocess.STDOUT)
# 	# wait for process to finish executing
# 	result.wait()
# 	print(score())
# 	# increment things TODO
# 	lp += 1
# 	test_counter += 1


# brute force ------------------------------------------------------------------------
best = 0
best_cmd = ''

for i in range(len(STATORS)):
	ls = i
	for j in range(len(STATORS)):
		# the stators must be different from each other
		if j is not i:
			rs = j
			for k in range(len(ORIENTATIONS)):
				lo = k
				for l in range(len(ORIENTATIONS)):
					ro = l
					for m in range(len(ALPHA)):
						lp = m
						for n in range(len(ALPHA)):
							rp = n
							command = ' '.join(get_command())
							print(command)
							# spawn new process
							result = subprocess.Popen(command, shell=True,
								                           stdout=subprocess.PIPE, 
								                           stderr=subprocess.STDOUT)
							# wait for process to finish executing
							result.wait()
							current_score = score()
							if current_score >= best:
								best = current_score
								best_cmd = command
								# print(current_score,i,j,k,l,m,n)

print(best_cmd)

