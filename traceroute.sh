#!/bin/bash
echo Taiwan | tee -a /home/shao/MWNL/MWNL/shao/result.txt
traceroute ncu.edu.tw | tee -a /home/shao/MWNL/MWNL/shao/result.txt
echo Europe | tee -a /home/shao/MWNL/MWNL/shao/result.txt
traceroute cam.ac.uk | tee -a /home/shao/MWNL/MWNL/shao/result.txt
echo America | tee -a /home/shao/MWNL/MWNL/shao/result.txt
traceroute amazon.com | tee -a /home/shao/MWNL/MWNL/shao/result.txt
echo | tee -a /home/shao/MWNL/MWNL/shao/result.txt
