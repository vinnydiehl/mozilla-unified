#!/usr/bin/env python
# coding=UTF-8

import mozunit
import pytest
from conftest import fspath


def test_no_dump_files(check_for_crashes):
    """Test that check_for_crashes returns 0 if no dumps are present."""
    assert 0 == check_for_crashes()


@pytest.mark.parametrize("minidump_files", [3], indirect=True)
def test_dump_count(check_for_crashes, minidump_files):
    """Test that check_for_crashes returns the number of crash dumps."""
    assert 3 == check_for_crashes()


def test_dump_directory_unicode(request, check_for_crashes, tmpdir, capsys):
    """Test that check_for_crashes can handle unicode in dump_directory."""
    from conftest import minidump_files

    tmpdir = tmpdir.ensure("🍪", dir=1)
    minidump_files = minidump_files(request, tmpdir)

    assert 1 == check_for_crashes(dump_directory=fspath(tmpdir), quiet=False)

    out, _ = capsys.readouterr()
    assert fspath(minidump_files[0]["dmp"]) in out
    assert "🍪" in out


def test_test_name_unicode(check_for_crashes, minidump_files, capsys):
    """Test that check_for_crashes can handle unicode in dump_directory."""
    assert 1 == check_for_crashes(test_name="🍪", quiet=False)

    out, err = capsys.readouterr()
    assert "| 🍪" in out


if __name__ == "__main__":
    mozunit.main()
