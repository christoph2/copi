
from copi import CSystemInformation

print(si = CSystemInformation())
print(si.getPageSize())
print(si.getAllocationGranularity())
print(si.getNumberOfProcessors())
print(si.isWow64())

def test_dummy():
    assert 1 == 1

