#!/usr/bin/env python
"""This integration test uses synthetic data and compares to expected results"""

import os
from integrationtest import IntegrationTest, main
from testdata import TEST_DATA_DIRECTORY
import unittest
import subprocess

class TestDiagnoseDups(IntegrationTest, unittest.TestCase):
    def setUp(self):
        IntegrationTest.setUp(self)
        self.data_dir = TEST_DATA_DIRECTORY
        self.orig_path = os.path.realpath(os.getcwd())
        self.exe_path = os.path.realpath(self.exe_path)
        os.chdir(self.data_dir)

    def tearDown(self):
        IntegrationTest.tearDown(self)
        os.chdir(self.orig_path)

    def test_program(self):
        expected_file = "expected_data.json"
        output_file = self.tempFile("output.json")
        input_bam = "integration_test.bam"
        cmdline = " ".join([self.exe_path, input_bam, '>', output_file])
        print "Executing: ", cmdline
        print "CWD: ", os.getcwd()

        rv = subprocess.call(cmdline, shell=True)
        print "Return value: ", rv
        self.assertEqual(0, rv)
        self.assertFilesEqual(expected_file, output_file)

if __name__ == "__main__":
    main()
