#!/bin/python

import numpy

data = numpy.loadtxt("block_log")
if data.size > 0:
    print "block log"
    print "mean {}".format(numpy.mean(data))
    print "standard deviation {}".format(numpy.std(data))

data = numpy.loadtxt("version_log")
if data.size > 0:
    print "version log"
    print "mean {}".format(numpy.mean(data))
    print "standard deviation {}".format(numpy.std(data))

data = numpy.loadtxt("reader_f_log")
if data.size > 0:
    print "pread file reader"
    print "mean {}".format(numpy.mean(data))
    print "standard deviation {}".format(numpy.std(data))
    print "count {}".format(data.size)

data = numpy.loadtxt("reader_m_log")
if data.size > 0:
    print "mmap file reader"
    print "mean {}".format(numpy.mean(data))
    print "standard deviation {}".format(numpy.std(data))
    print "count {}".format(data.size)

with open("reader_log") as logfile:
    array_start = []
    array_read = []
    array_finish = []
    for line in logfile:
        line = line.split(" ")

        if line[0] == "START":
            array_start.append(line[1])

        elif line[0] == "READ":
            array_read.append(line[1])

        elif line[0] == "FINISH":
            array_finish.append(line[1])

    print "block reader details"
    print "start mean {}".format(numpy.mean(numpy.array(array_start).astype(numpy.int)))
    print "read mean {}".format(numpy.mean(numpy.array(array_read).astype(numpy.int)))
    print "finish mean {}".format(numpy.mean(numpy.array(array_finish).astype(numpy.int)))
    print "start deviation {}".format(numpy.std(numpy.array(array_start).astype(numpy.int)))
    print "read deviation {}".format(numpy.std(numpy.array(array_read).astype(numpy.int)))
    print "finish deviation {}".format(numpy.std(numpy.array(array_finish).astype(numpy.int)))
