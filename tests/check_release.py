#!/usr/bin/env python3
"""Exercise release input failures and archive replacement on the host."""
import importlib.util
import shutil
import struct
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('release', ROOT / 'tools/release.py')
release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(release)
from prepare_aminet import prepare


class ReleaseTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='mushin-release-')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.build = self.root / 'objects with spaces'
        self.output = self.root / 'release with spaces'
        self.build.mkdir()
        self.output.mkdir()
        for name in release.LIBRARIES:
            self.library(name, '35.6')
        (self.build / 'opentest').write_bytes(struct.pack('>I', 1011))

    def library(self, name, version):
        (self.build / name).write_bytes(struct.pack('>I', 1011) +
                                       f'$VER: {name} {version} (test)\0'.encode())

    def test_mismatched_versions(self):
        self.library('muimaster.library', '35.4')
        with self.assertRaisesRegex(ValueError, 'versions differ'):
            release.release(self.build, self.output, 'lha', full=False)
        self.assertEqual(list(self.output.iterdir()), [])

    def test_missing_version(self):
        (self.build / 'muimaster.library').write_bytes(struct.pack('>I', 1011))
        with self.assertRaisesRegex(ValueError, 'matching'):
            release.release(self.build, self.output, 'lha', full=False)

    def test_wrong_library_name(self):
        shutil.copyfile(self.build / 'muimaster.library',
                        self.build / 'zunemaster.library')
        with self.assertRaisesRegex(ValueError, 'matching'):
            release.release(self.build, self.output, 'lha', full=False)

    def test_missing_tool(self):
        with self.assertRaisesRegex(ValueError, 'archiver not found'):
            release.release(self.build, self.output, str(self.root / 'absent'), full=False)

    def test_incomplete_full_package(self):
        for name in ['SDK/include/example.h', 'Docs/COPYING']:
            path = self.build / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b'fixture')
        for name in ['Prefs/Zune', 'Prefs/Zune.info']:
            with self.assertRaisesRegex(ValueError, 'missing ' + name):
                release.release(self.build, self.output, 'lha')
            path = self.build / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b'fixture')
        with self.assertRaisesRegex(ValueError, 'missing Libs/MUI/BetterString.mcc'):
            release.release(self.build, self.output, 'lha')
        self.assertEqual(list(self.output.iterdir()), [])

    def test_archiver_failure_preserves_previous_release(self):
        archive = self.output / 'Mushin-35.6-amigaos3-m68k.lha'
        archive.write_bytes(b'previous release')
        with self.assertRaises(release.subprocess.CalledProcessError):
            release.release(self.build, self.output, '/usr/bin/false', full=False)
        self.assertEqual(archive.read_bytes(), b'previous release')
        self.assertEqual(list(self.output.iterdir()), [archive])

    def test_aminet_pair_and_version_guard(self):
        archive = self.output / 'Mushin-35.6-amigaos3-m68k.lha'
        archive.write_bytes(b'archive fixture')
        readme = archive.with_suffix('.readme')
        readme.write_text('Short: Mushin\nType: util/libs\nVersion: 35.6\n\nBody\n')
        destination = self.root / 'aminet'
        prepare(self.output, destination, 'v35.6')
        self.assertEqual((destination / 'Mushin.lha').read_bytes(), archive.read_bytes())
        self.assertEqual((destination / 'Mushin.readme').read_bytes(), readme.read_bytes())
        original = (destination / 'Mushin.readme').read_bytes()
        readme.write_text('Type: util/libs\nVersion: 35.5\n')
        with self.assertRaisesRegex(ValueError, 'version differ'):
            prepare(self.output, destination, 'v35.6')
        self.assertEqual((destination / 'Mushin.readme').read_bytes(), original)
        with self.assertRaisesRegex(ValueError, 'version tag'):
            prepare(self.output, destination, '../v35.6')


if __name__ == '__main__':
    unittest.main()
